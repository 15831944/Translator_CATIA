#include "stdafx.h"
#include "FeatureSOLIDCreateExtrude.h"
#include "FSketch.h"
#include "ReferenceEntity.h"
namespace Pre {

	FeatureSOLIDCreateExtrude::FeatureSOLIDCreateExtrude(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		_startDepth = 0;	// �ʱⰪ 0
		_endDepth = 0;		// �ʱⰪ 0
		_startCondition = TransCAD::Blind;	// �ʱⰪ Blind
		_endCondition = TransCAD::Blind;	// �ʱⰪ Blind
		_flip = VARIANT_FALSE;			// �ʱⰪ false

		_featureProfileSketch = 0;
	}

	FeatureSOLIDCreateExtrude::~FeatureSOLIDCreateExtrude(void)
	{
	}

	void FeatureSOLIDCreateExtrude::GetInfo(ifstream & is)
	{
		char buffer[500];

		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set pad1�� ã�� �κ�
		{
			is.getline(buffer, 500);
		}

		string sketchManagerName;
		char seps_temp[] = " ,\t\n().="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		sketchManagerName = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //����° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ�

		if (sketchManagerName == "AddNewPad" || sketchManagerName == "AddNewPocket")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// SketchManager�� �̸����� ���ϴ� Sketch ������ �����´�.	
			_featureProfileSketch = (FSketch*)GetPart()->GetFeatureByName(sketchManagerName);

			// Feature�� Sketch������ �����Ѵ�.
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketchManagerName);
		}
		else if (sketchManagerName == "AddNewPadFromRef" || sketchManagerName == "AddNewPocketFromRef")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// Reference�� �����Ѵ�.
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(sketchManagerName));

			// Feature�� Sketch������ �����Ѵ�. ��, Sketch ������ �ִ� ���
			sketchManagerName = _pPart->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			if (!sketchManagerName.empty())
				GetPart()->AddFeatureNameSketchName(GetFeatureName(), sketchManagerName);
		}

		char seps_temp2[] = " ,\t\n()="; //������
		sketchManagerName = strtok_s(NULL, seps_temp2, &context);
		_startDepth = atof(sketchManagerName.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreateExtrude::ToTransCAD()
	{
		TransCAD::IReferencePtr spBaseSketch;

		if (_featureProfileSketch)
		{
			bstr_t baseSketchName(GetFeatureProfileSketch()->GetFeatureName().c_str());
			spBaseSketch = GetPart()->_spPart->SelectObjectByName(baseSketchName);
		}
		else
		{
			vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();
			while (iter != _referenceEntityList.end())
			{
				(*iter)->ToTransCAD();
				if ((*iter)->GetReferencePtr() != NULL)
					spBaseSketch = (*iter)->GetReferencePtr();

				iter++;
			}
		}

		if (_type == 1)
			// Create a protrusion extrude feature with the sketch
			GetPart()->_spFeatures->AddNewSolidProtrusionExtrudeFeature(_name.c_str(), spBaseSketch, _startDepth, _startCondition, _endDepth, _endCondition, _flip);
		else
			// Create a cut extrude feature with the sketch
			GetPart()->_spFeatures->AddNewSolidCutExtrudeFeature(_name.c_str(), spBaseSketch, _startDepth, _startCondition, _endDepth, _endCondition, _flip);
	}

	void FeatureSOLIDCreateExtrude::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		token = strtok_s(buffer, seps, &context);
		token = strtok_s(NULL, seps, &context);

		if (token == "DirectionOrientation") // ���⼭ Flip ���θ� ó��
		{
			token = strtok_s(NULL, seps, &context);

			if (token == "catInverseOrientation")
			{
				if (_type == 1)
					_flip = VARIANT_TRUE;
				else if (_type == 2)
					_flip = VARIANT_FALSE;
			}
			else if (token == "catRegularOrientation")
			{
				if (_type == 1)
					_flip = VARIANT_FALSE;
				else if (_type == 2)
					_flip = VARIANT_TRUE;
			}
		}
		else if (token == "IsSymmetric")
		{
			token = strtok_s(NULL, seps, &context);

			if (token == "True")
				_endDepth = _startDepth;
			else if (token == "False")
				_endDepth = 0;
		}
		else if (token == "SetProfileElement")
		{
			token = strtok_s(NULL, seps, &context);

			// ReferenceElement ������ vector�� ����.	
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(token));

			// Feature�� Sketch������ �����Ѵ�. ��, Sketch ������ �ִ� ���
			token = _pPart->GetReferenceEntityByName(token)->GetFeatureName();
			if (!token.empty())
				GetPart()->AddFeatureNameSketchName(GetFeatureName(), token);
		}

		context = NULL;
	}

	void FeatureSOLIDCreateExtrude::CheckAttribute(string name, double value, int type)
	{
		if (name == "FirstLimit")
		{
			if (type == 0)
			{
				_startDepth = value;
				_startCondition = TransCAD::Blind;	// 0: catOffsetLimit
			}
			else if (type == 1 || type == 2)			// 1: catUpThruNextLimit, 2: catUpToLastLimit
			{
				_startDepth = 0;
				_startCondition = TransCAD::ThroughAll;
			}
			else if (type == 3 || type == 4)			// 3: catUpToPlaneLimit, 4: catUpToSurfaceLimit. ���� �ʿ�
			{
				_startDepth = 0;
				_startCondition = TransCAD::ThroughAll;
			}
		}
		else if (name == "SecondLimit")
		{
			if (type == 0)
			{
				_endDepth = value;
				_endCondition = TransCAD::Blind;	// 0: catOffsetLimit
			}
			else if (type == 1 || type == 2)			// 1: catUpThruNextLimit, 2: catUpToLastLimit
			{
				_endDepth = 0;
				_endCondition = TransCAD::ThroughAll;
			}
			else if (type == 3 || type == 4)			// 3: catUpToPlaneLimit, 4: catUpToSurfaceLimit. ���� �ʿ�
			{
				_endDepth = 0;
				_endCondition = TransCAD::ThroughAll;
			}
		}
	}
}