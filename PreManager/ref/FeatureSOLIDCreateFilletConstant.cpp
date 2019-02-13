#include "stdafx.h"
#include "FeatureSOLIDCreateFilletConstant.h"
#include "ReferenceEntity.h"

namespace Pre {
	FeatureSOLIDCreateFilletConstant::FeatureSOLIDCreateFilletConstant(Part * pPart, int type, string name) :
		Feature(pPart, type, name)
	{
		_propagationType = TransCAD::Minimal;
	}

	FeatureSOLIDCreateFilletConstant::~FeatureSOLIDCreateFilletConstant(void)
	{
	}

	void FeatureSOLIDCreateFilletConstant::GetInfo(ifstream & is)
	{
		// Chamfer�� ���� �Է�
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set chamfer1�� ã�� �κ�
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string filletValue;
		char seps_temp[] = " ,\t\n()="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		filletValue = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ�
		filletValue = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�
		filletValue = strtok_s(NULL, seps_temp, &context);  //����° �ܾ�
		filletValue = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ� : reference name

		// ReferenceElement ������ feature�� ����.	
		_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(filletValue));

		filletValue = strtok_s(NULL, seps_temp, &context);  //propagation
		_propagation = atoi(filletValue.c_str());

		filletValue = strtok_s(NULL, seps_temp, &context);  //length1
		_radius = atof(filletValue.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreateFilletConstant::ToTransCAD()
	{
		TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();
		vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();

		while (iter != _referenceEntityList.end())
		{
			(*iter)->ToTransCAD();
			if ((*iter)->GetReferencePtr() != NULL)
				spReferences->Add((*iter)->GetReferencePtr()); //�ټ��� ���۷���(����) ���� �� �߰�
			iter++;
		}

		if (_propagation == 0) //minimal
			_propagationType = TransCAD::Minimal;
		else if (_propagation == 1) //tangent
			_propagationType = TransCAD::Tangency;
		else
		{
		}

		// Create a fillet constant feature
		GetPart()->_spFeatures->AddNewSolidFilletConstantFeature(_name.c_str(), spReferences, _radius, _propagationType);
	}

	void FeatureSOLIDCreateFilletConstant::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		token = strtok_s(buffer, seps, &context);  //chamfer1
		token = strtok_s(NULL, seps, &context); //�ι�° ������

		if (token == "AddObjectToFillet")
		{
			token = strtok_s(NULL, seps, &context); //����° ������

			// ReferenceElement ������ vector�� ����.	
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(token));
		}

		if (token == "EdgePropagation")
		{
			token = strtok_s(NULL, seps, &context); //����° ������

			if (token == "catMinimalFilletEdgePropagation")
				_propagation = 0;
			else if (token == "catTangencyFilletEdgePropagation")
				_propagation = 1;
		}
	}

	void FeatureSOLIDCreateFilletConstant::CheckAttribute(string name, double value, int type)
	{
		if (name == "Radius")
		{
			_radius = value;
		}
	}
}