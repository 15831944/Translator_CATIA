#include "stdafx.h"
#include "FeatureSOLIDCreateChamfer.h"
#include "ReferenceEntity.h"

namespace Pre {
	FeatureSOLIDCreateChamfer::FeatureSOLIDCreateChamfer(Part * pPart, int type, string name) :
		Feature(pPart, type, name)
	{
	}

	FeatureSOLIDCreateChamfer::~FeatureSOLIDCreateChamfer(void)
	{
	}

	void FeatureSOLIDCreateChamfer::GetInfo(ifstream & is)
	{
		// Chamfer�� ���� �Է�
		char buffer[500];
		is.getline(buffer, 500);

		while (strncmp(buffer, "Set", 3))  // Set chamfer1�� ã�� �κ�
		{
			is.getline(buffer, 500);
		}

		string chamferValue;
		char seps_temp[] = " ,\t\n()="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		chamferValue = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ�
		chamferValue = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�
		chamferValue = strtok_s(NULL, seps_temp, &context);  //����° �ܾ�

		chamferValue = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ� : reference name

		// ReferenceElement ������ feature�� ����.	
		_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(chamferValue));

		chamferValue = strtok_s(NULL, seps_temp, &context);  //propagation
		_propagation = atoi(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //mode
		_mode = atoi(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //orientation
		_orientation = atoi(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //length1
		_length1 = atof(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //length2
		_length2 = atof(chamferValue.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreateChamfer::ToTransCAD()
	{
		cerr << "#-- Call Chamfer::Translate2TransCAD() --#" << endl;

		TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();
		vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();

		while (iter != _referenceEntityList.end())
		{
			(*iter)->ToTransCAD();
			if ((*iter)->GetReferencePtr() != NULL)
				spReferences->Add((*iter)->GetReferencePtr()); //�ټ��� ���۷���(����) ���� �� �߰�
			iter++;
		}

		GetPart()->_spFeatures->AddNewSolidChamferFeature(_name.c_str(), spReferences, _length1);

		cerr << "#-- End Chamfer::Translate2TransCAD() --#" << endl << endl;
	}

	void FeatureSOLIDCreateChamfer::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		token = strtok_s(buffer, seps, &context);  //chamfer1
		token = strtok_s(NULL, seps, &context); //�ι�° ������

		if (token == "AddElementToChamfer")
		{
			token = strtok_s(NULL, seps, &context); //����° ������

			// ReferenceElement ������ vector�� ����.	
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(token));
		}

		if (token == "Mode")
		{
			token = strtok_s(NULL, seps, &context); //����° ������
			_mode = atoi(token.c_str());
		}

		if (token == "Propagation")
		{
			token = strtok_s(NULL, seps, &context); //����° ������
			_propagation = atoi(token.c_str());
		}

		if (token == "Orientation")
		{
			token = strtok_s(NULL, seps, &context); //����° ������
			_orientation = atoi(token.c_str());
		}
	}

	void FeatureSOLIDCreateChamfer::CheckAttribute(string name, double value, int type)
	{
		if (name == "Length1")
		{
			_length1 = value;
		}

		if (name == "Length2")
		{
			_length2 = value;
		}

		if (name == "Angle")
		{
			_length2 = value;
		}
	}
}