#include "stdafx.h"
#include "FDatumPlaneOffset.h"
#include "ReferenceEntity.h"

namespace Pre {
	FDatumPlaneOffset::FDatumPlaneOffset(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		_distance = 0;
		_flip = 0;
	}

	FDatumPlaneOffset::~FDatumPlaneOffset()
	{
	}

	void FDatumPlaneOffset::GetInfo(ifstream &is)
	{
		// Chamfer�� ���� �Է�
		char buffer[500];
		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set chamfer1�� ã�� �κ�
		{
			is.getline(buffer, 500);
		}

		string planeOffsetValue;
		char seps_temp[] = " ,\t\n()="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		planeOffsetValue = strtok_s(buffer, seps_temp, &context); //Set
		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //hybridShapePlaneOffset1
		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //hybridShapeFactory1.AddNewPlaneOffset

		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //reference1

		// ReferenceElement ������ feature�� ����.	
		// ó�� reference entity�� �̸��� �а� �ٽ� �� �̸��� ������ �ٸ� reference entity�� �̸��� �����´�.

		string target_surface;
		target_surface = GetPart()->GetReferenceEntityByName(planeOffsetValue)->GetFeatureName();

		if (GetPart()->GetReferenceEntityByName(target_surface))
			_referenceEntityList.push_back(GetPart()->GetReferenceEntityByName(target_surface));
		else
			_referenceEntityList.push_back(GetPart()->GetReferenceEntityByName(planeOffsetValue));


		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //20.000000
		_distance = atof(planeOffsetValue.c_str());

		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //False

		if (planeOffsetValue == "False")
		{
			_flip = 0;
		}
		else
		{
			_flip = 1;
		}

		context = NULL;
	}

	void FDatumPlaneOffset::ToTransCAD()
	{
		cerr << "#-- Call DatumPlaneOffset::Translate2TransCAD() --#" << endl;

		TransCAD::IReferencePtr spPlane;

		vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();
		while (iter != _referenceEntityList.end())
		{
			(*iter)->ToTransCAD();

			if ((*iter)->GetReferencePtr() != NULL)
				spPlane = (*iter)->GetReferencePtr();

			iter++;
		}

		// Create a datum plane offset feature
		GetPart()->_spFeatures->AddNewDatumPlaneOffsetFeature(_name.c_str(), spPlane, _distance, _flip);

		cerr << "#-- End DatumPlaneOffset::Translate2TransCAD() --#" << endl << endl;
	}

	void FDatumPlaneOffset::Modify(char * buffer) //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
	}

	void FDatumPlaneOffset::CheckAttribute(string name, double value, int type)
	{
	}




}