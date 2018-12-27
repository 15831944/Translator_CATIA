#include "stdafx.h"
#include "FeatureSOLIDCreatePatternRectangular.h"
#include "ReferenceEntity.h"

namespace Pre {
	FeatureSOLIDCreatePatternRectangular::FeatureSOLIDCreatePatternRectangular(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		ColumnSpacing = 0;
		ColumnNumber = 0;
		ColDirX = 0;
		ColDirY = 0;
		ColDirZ = 0;

		RowSpacing = 0;
		RowNumber = 0;
		RowDirX = 0;
		RowDirY = 0;
		RowDirZ = 0;
	}

	FeatureSOLIDCreatePatternRectangular::~FeatureSOLIDCreatePatternRectangular()
	{
	}

	void FeatureSOLIDCreatePatternRectangular::GetInfo(ifstream &is)
	{
		// Chamfer�� ���� �Է�
		char buffer[400];
		char * context = NULL;
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set chamfer1�� ã�� �κ�
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string rectpatternValue;
		char seps_temp[] = " ,\t\n()="; //������
		rectpatternValue = strtok_s(buffer, seps_temp, &context);//ù��° �ܾ�
		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�
		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //����° �ܾ�

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ� 

		_spTargetFeature = rectpatternValue.c_str();				// Pattern�� ���� �� Target Feature

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //ColumnNumber
		ColumnNumber = atoi(rectpatternValue.c_str());

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //RowNumber
		RowNumber = atoi(rectpatternValue.c_str());

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //ColumnSpacing
		ColumnSpacing = atoi(rectpatternValue.c_str());

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //RowSpacing
		RowSpacing = atof(rectpatternValue.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreatePatternRectangular::ToTransCAD()
	{
		cerr << "#-- Call RectPattern::Translate2TransCAD() --#" << endl;

		cerr << "RectPattern Name:" << _name.c_str() << endl;

		// ���� ���� ����
		SetDirection(ColumnSpacing, RowSpacing, _RefPlane);

		TransCAD::IReferencePtr spTargetFeature = GetPart()->_spPart->SelectFeatureByName(_spTargetFeature);
		TransCAD::IReferencesPtr spTargetFeatures = GetPart()->_spPart->CreateReferences();
		spTargetFeatures->Add(spTargetFeature);

		GetPart()->_spFeatures->AddNewSolidOperatePatternRectangularFeature(_name.c_str(), spTargetFeatures, ColumnSpacing, ColDirX, ColDirY, ColDirZ, ColumnNumber,
			RowSpacing, RowDirX, RowDirY, RowDirZ, RowNumber);

		cerr << "#-- End RectPattern::Translate2TransCAD() --#" << endl << endl;
	}

	void FeatureSOLIDCreatePatternRectangular::Modify(char * buffer)  //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
		string token;
		char * context = NULL;
		char seps[] = " ,\t\n().="; //������
		token = strtok_s(buffer, seps, &context);	//rectPattern1
		token = strtok_s(NULL, seps, &context);		//SetFirstDirection

		int _type;

		if (token == "SetFirstDirection")
		{
			token = strtok_s(NULL, seps, &context);		//reference11
			_firstDirection = token;

			_type = _pPart->GetReferenceEntityByName(token)->GetReferenceType();
		}
		else if (token == "SetSecondDirection")
		{
			token = strtok_s(NULL, seps, &context);		//reference11
			_secondDirection = token;

			_type = _pPart->GetReferenceEntityByName(token)->GetReferenceType();
		}
		else if (token == "FirstOrientation")
		{
			token = strtok_s(NULL, seps, &context);		//reference11

			if (token == "False")
				ColumnSpacing = (-1)*ColumnSpacing;

		}
		else if (token == "SecondOrientation")
		{
			token = strtok_s(NULL, seps, &context);		//reference11

			if (token == "False")
				RowSpacing = (-1)*RowSpacing;
		}

		// ���۷����� ��� (Plane Ȥ�� Face)�� ��� ����
		// ����̸� �� ������ �ݵ�� ���� ����� �����Ѵ�.
		if (_type == 1 || _type == 3 || _type == 4)
			_RefPlane = token;

		context = NULL;
	}

	void FeatureSOLIDCreatePatternRectangular::CheckAttribute(string name, double value, int type)
	{
		if (name == "FirstDirectionRepartition")
		{
			if (type == 0)
				ColumnNumber = (int)value;
			else if (type == 1)
				ColumnSpacing = (double)value;
		}
		else if (name == "SecondDirectionRepartition")
		{
			if (type == 0)
				RowNumber = (int)value;
			else if (type == 1)
				RowSpacing = (double)value;
		}
	}

	void FeatureSOLIDCreatePatternRectangular::SetDirection(double colspacing, double rowspacing, string refplane)
	{
		// RefPlane���� �������� ColDir, RowDir�� ������ Ȯ��. 
		// ������� PlaneXY�� ��� ColDirX = 1, RowDirY = 1�� ��.
		// ����, �Է¹��� Spacing ���� ���� �� ��� Direction�� ����
		string RefPlane = _pPart->GetReferenceEntityByName(refplane)->GetFeatureName();
		if (RefPlane == "PlaneXY")
		{
			ColDirX = 1;
			RowDirY = 1;

			if (colspacing < 0)
				ColDirX = ColDirX * (-1);
			if (rowspacing < 0)
				RowDirY = RowDirY * (-1);
		}
		else if (RefPlane == "PlaneYZ")
		{
			ColDirY = 1;
			RowDirZ = 1;

			if (colspacing < 0)
				ColDirY = ColDirY * (-1);
			if (rowspacing < 0)
				RowDirZ = RowDirZ * (-1);
		}
		else if (RefPlane == "PlaneZX")
		{
			ColDirZ = 1;
			RowDirX = 1;

			if (colspacing < 0)
				ColDirZ = ColDirZ * (-1);
			if (rowspacing < 0)
				RowDirX = RowDirX * (-1);
		}
		else
		{	// ����� DatumPlaneOffset�� ��츸 ó��
			// �� ��, Face�� Edge�� ��쿡�� ó���ؾ� ��
			string RefPlane1 = _pPart->GetReferenceEntityByName(RefPlane)->GetFeatureName();
			if (RefPlane1 == "PlaneXY")
			{
				ColDirX = 1;
				RowDirY = 1;

				if (colspacing < 0)
					ColDirX = ColDirX * (-1);
				if (rowspacing < 0)
					RowDirY = RowDirY * (-1);
			}
			else if (RefPlane1 == "PlaneYZ")
			{
				ColDirY = 1;
				RowDirZ = 1;

				if (colspacing < 0)
					ColDirY = ColDirY * (-1);
				if (rowspacing < 0)
					RowDirZ = RowDirZ * (-1);
			}
			else if (RefPlane1 == "PlaneZX")
			{
				ColDirZ = 1;
				RowDirX = 1;

				if (colspacing < 0)
					ColDirZ = ColDirZ * (-1);
				if (rowspacing < 0)
					RowDirX = RowDirX * (-1);
			}
		}

		// Spacing(����)�� �����̱� ������ ���� ����
		ColumnSpacing = abs(ColumnSpacing);
		RowSpacing = abs(RowSpacing);
	}





}