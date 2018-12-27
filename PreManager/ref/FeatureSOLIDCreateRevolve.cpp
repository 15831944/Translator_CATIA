#include "stdafx.h"
#include "FeatureSOLIDCreateRevolve.h"
#include "ReferenceEntity.h"
#include "FSketchItem.h"
#include "FSKETCHCreate2DLine2Points.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"

//using namespace Pre;

namespace Pre {

	FeatureSOLIDCreateRevolve::FeatureSOLIDCreateRevolve(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		startAngle = 360;	//���۰��� 360.
		endAngle = 0;		//������ 0�̴�.
		startCondition = "Dimension";
		endCondition = "Dimension";
		flip = 0;				//flip�� ����.

		_featureProfileSketch = 0;
	}

	FeatureSOLIDCreateRevolve::~FeatureSOLIDCreateRevolve()
	{
	}

	void FeatureSOLIDCreateRevolve::GetInfo(ifstream &is)
	{
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set pad1�� ã�� �κ�
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string sketchManagerName;
		char seps_temp[] = " ,\t\n().="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		sketchManagerName = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //����° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ�

		if (sketchManagerName == "AddNewShaft" || sketchManagerName == "AddNewGroove")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// SketchManager�� �̸����� ���ϴ� Sketch ������ �����´�.	
			_featureProfileSketch = (FSketch*)GetPart()->GetFeatureByName(sketchManagerName);

			// Feature�� Sketch������ �����Ѵ�.
			GetPart()->AddFeatureNameSketchName(GetFeatureName(), sketchManagerName);
		}
		else if (sketchManagerName == "AddNewShaftFromRef" || sketchManagerName == "AddNewGrooveFromRef")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// Reference�� �����Ѵ�.
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(sketchManagerName));

			// Feature�� Sketch������ �����Ѵ�. ��, Sketch ������ �ִ� ���
			sketchManagerName = _pPart->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			if (!sketchManagerName.empty())
				GetPart()->AddFeatureNameSketchName(GetFeatureName(), sketchManagerName);
		}

		context = NULL;
	}

	void FeatureSOLIDCreateRevolve::ToTransCAD()
	{
		// ����ġ �ܺο��� ȸ�� ���� ���� ���, �� ���� ����ġ ���ο� ����
		AddNewRevolveAxis();

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

		if (_type == 5)
			// Create a protrusion Revolve feature with the sketch
			GetPart()->_spFeatures->AddNewSolidProtrusionRevolveFeature(_name.c_str(), spBaseSketch, startAngle, TransCAD::StdRevolveEndType_Blind, endAngle, TransCAD::StdRevolveEndType_Blind, false);
		else
			// Create a cut Revolve feature with the sketch
			GetPart()->_spFeatures->AddNewSolidCutRevolveFeature(_name.c_str(), spBaseSketch, startAngle, TransCAD::StdRevolveEndType_Blind, endAngle, TransCAD::StdRevolveEndType_Blind, false);
	}

	void FeatureSOLIDCreateRevolve::CheckAttribute(string name, double value, int type)
	{
		if (name == "FirstAngle")
		{
			startAngle = value;
		}
		else if (name == "SecondAngle")
		{
			endAngle = value;
		}
	}

	void FeatureSOLIDCreateRevolve::Modify(char * buffer)
	{
		string token;
		char * context = NULL;
		char seps[] = " ,\t\n().=";		//������

		token = strtok_s(buffer, seps, &context);		//hole1
		token = strtok_s(NULL, seps, &context);		//�ι�° ������

		if (token == "RevoluteAxis")
		{
			token = strtok_s(NULL, seps, &context);	//����° ������

			_revolveAxis = token; // reference
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
	}

	void FeatureSOLIDCreateRevolve::AddNewRevolveAxis()
	{
		if (!_revolveAxis.empty())
		{
			// ���۷��� ���� ����ġ
			string _revRefSketName = GetPart()->GetReferenceEntityByName(_revolveAxis)->GetWireInfo()._sketchName;		// ����ġ �̸�
			string _revRefEntName = GetPart()->GetReferenceEntityByName(_revolveAxis)->GetWireInfo()._sketchItemName;	// ����ġ�� ���� �� ����. ��, ȸ�� ��
			FSketch * pRefSketch = (FSketch*)GetPart()->GetFeatureByName(_revRefSketName);								// ���۷��� ����ġ

			string _shaftSketName = GetPart()->GetSketchNameFromFeatureName(_name);			// Revolve ����ġ �̸�
			FSketch * pRevSketch = (FSketch*)GetPart()->GetFeatureByName(_shaftSketName);	// Revolve ����ġ

			if (_revRefSketName.find("sketch") == 0)
			{
				// �Ϲ����� ���.
				// ����ġ ������ �ϳ��� Line�� Centerline���� ������
				if (_revRefSketName == _shaftSketName)
				{
					FSKETCHCreate2DLine2Points * pRefLine = (FSKETCHCreate2DLine2Points*)pRefSketch->GetSketchItemFromName(_revRefEntName);
					origin = pRefLine->GetStartPoint();
					direction = pRefLine->GetEndPoint();

				}
				// Revolve�� Sketch�� Revolve Axis�� sketch�� �ٸ� ���
				// ���� ���, �ٸ� ������� Axis�� ���� �� ���
				else
				{
					// Circle�� Extrude�� ������� ���
					if (_revRefEntName.find("circle2D") == 0)
					{
						FSKETCHCreate2DCircleCenterPoint * pRefCircle = (FSKETCHCreate2DCircleCenterPoint*)pRefSketch->GetSketchItemFromName(_revRefEntName);

						// circle�� ����. ��, �� ���� Local ��ǥ
						pRefCircle->GetCenterPoint(origin);

						// circle ������ normal ����
						direction.SetX(origin.X());
						direction.SetY(origin.Y());
						direction.SetZ(origin.Z() + 1.0);
					}
					// �Ϲ� ����ġ�� Revolve�� ȸ��ü�� ���
					else
					{
						string refFeatName = GetPart()->GetFeatureNameFromSketchName(_revRefSketName);

						FeatureSOLIDCreateRevolve * pRevFeature = (FeatureSOLIDCreateRevolve *)GetPart()->GetFeatureByName(refFeatName);
						origin = pRevFeature->GetAxisOrigin();
						direction = pRevFeature->GetAxisDirection();
					}
				}
			}
			// ����ġ ������ Local ��ǥ�踦 Axis�� ��� ���
			else if (_revRefSketName.find("axis2D") == 0)
			{
				_revRefSketName = "sketch" + _revRefSketName.substr(6, 1);
				FSketch * pRevSketch = (FSketch*)GetPart()->GetFeatureByName(_revRefSketName);

				if (_revRefEntName == "HDirection")
					direction.SetX(1.0);
				else if (_revRefEntName == "VDirection")
					direction.SetY(1.0);
			}

			//���ο� Centerline ����
			CreateRevolveAxis(pRefSketch, pRevSketch, origin, direction);
		}
	}

	void FeatureSOLIDCreateRevolve::CreateRevolveAxis(FSketch * pRefSketch, FSketch * pRevSketch, Point3D origin, Point3D direction)
	{
		if (pRefSketch && (pRefSketch != pRevSketch))
		{
			TransformRevolveAxis(pRefSketch->sketchCoord, pRevSketch->sketchCoord, origin);
			TransformRevolveAxis(pRefSketch->sketchCoord, pRevSketch->sketchCoord, direction);
		}

		TransCAD::ISketchEditorPtr spSketEditor = pRevSketch->GetSketchFeaturePtr()->OpenEditorEx(VARIANT_FALSE);
		spSketEditor->Create2DCenterline2Points("ShaftAxis", origin.X(), origin.Y(), direction.X(), direction.Y());
	}

	void FeatureSOLIDCreateRevolve::TransformRevolveAxis(double refSketCSYS[9], double revSketCSYS[9], Point3D &coords)
	{
		double globalCSYS[3][3], localCSYS[3][3];

		// Global ��ǥ��
		globalCSYS[0][0] = 1; globalCSYS[0][1] = 0; globalCSYS[0][2] = 0;
		globalCSYS[1][0] = 0; globalCSYS[1][1] = 1; globalCSYS[1][2] = 0;
		globalCSYS[2][0] = 0; globalCSYS[2][1] = 0; globalCSYS[2][2] = 1;

		// Local ��ǥ�� (Reference ����ġ)
		localCSYS[0][0] = refSketCSYS[3]; localCSYS[0][1] = refSketCSYS[4]; localCSYS[0][2] = refSketCSYS[5];
		localCSYS[1][0] = refSketCSYS[6]; localCSYS[1][1] = refSketCSYS[7]; localCSYS[1][2] = refSketCSYS[8];
		localCSYS[2][0] = localCSYS[0][1] * localCSYS[1][2] - localCSYS[0][2] * localCSYS[1][1];
		localCSYS[2][1] = localCSYS[0][2] * localCSYS[1][0] - localCSYS[0][0] * localCSYS[1][2];
		localCSYS[2][2] = localCSYS[0][0] * localCSYS[1][1] - localCSYS[0][1] * localCSYS[1][0];

		// Local ��ǥ�� �� Global ��ǥ�� ��ȯ
		TransformCoordinates(localCSYS, globalCSYS, coords);

		// ��ǥ�� ���� �̵�
		// Reference ����ġ�� ������ŭ ��(+)�� �̵�(Local��Global), Revolve ����ġ�� ������ŭ ��(-)�� �̵�(Global��Local)
		coords.SetX(coords.X() + refSketCSYS[0] - revSketCSYS[0]);
		coords.SetY(coords.Y() + refSketCSYS[1] - revSketCSYS[1]);
		coords.SetZ(coords.Z() + refSketCSYS[2] - revSketCSYS[2]);

		// Local ��ǥ�� (Revolve ����ġ)	
		localCSYS[0][0] = revSketCSYS[3]; localCSYS[0][1] = revSketCSYS[4]; localCSYS[0][2] = revSketCSYS[5];
		localCSYS[1][0] = revSketCSYS[6]; localCSYS[1][1] = revSketCSYS[7]; localCSYS[1][2] = revSketCSYS[8];
		localCSYS[2][0] = localCSYS[0][1] * localCSYS[1][2] - localCSYS[0][2] * localCSYS[1][1];
		localCSYS[2][1] = localCSYS[0][2] * localCSYS[1][0] - localCSYS[0][0] * localCSYS[1][2];
		localCSYS[2][2] = localCSYS[0][0] * localCSYS[1][1] - localCSYS[0][1] * localCSYS[1][0];

		// Global ��ǥ�� �� Local ��ǥ�� ��ȯ
		TransformCoordinates(globalCSYS, localCSYS, coords);
	}

	void FeatureSOLIDCreateRevolve::TransformCoordinates(double sourceCSYS[3][3], double targetCSYS[3][3], Point3D &coords)
	{
		double sourceCoords[3], targetCoords[3], transMatrix[3][3];

		// sourceCSYS�� ��ǥ��
		sourceCoords[0] = coords.X();
		sourceCoords[1] = coords.Y();
		sourceCoords[2] = coords.Z();

		// Transformation matrix ���ϱ�
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				transMatrix[i][j] = 0;

				for (int k = 0; k < 3; ++k)
					transMatrix[i][j] = transMatrix[i][j] + targetCSYS[i][k] * sourceCSYS[j][k];
			}
		}

		// Transformation matrix�� ������� targetCSYS�� ��ǥ�� ���ϱ�
		for (int i = 0; i < 3; ++i)
		{
			targetCoords[i] = 0;

			for (int j = 0; j < 3; ++j)
				targetCoords[i] = targetCoords[i] + transMatrix[i][j] * sourceCoords[j];

		}

		coords.SetX(targetCoords[0]);
		coords.SetY(targetCoords[1]);
		coords.SetZ(targetCoords[2]);
	}
}