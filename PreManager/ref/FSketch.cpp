#include "stdafx.h"

#include "FSketch.h"
#include "FSKETCHItem.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"
#include "FSKETCHCreate2DLine2Points.h"
#include "FSKETCHCreate2DPoint.h"

#include "ReferenceEntity.h"

namespace Pre {
	FSketch::FSketch(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
	}

	FSketch::~FSketch(void)
	{
		_spSketEditor = NULL;
		_spSketch = NULL;

		Clear();
	}

	void FSketch::Clear()
	{
		std::vector<FSKETCHItem*>::iterator i = _sketchItemList.begin();
		while (i != _sketchItemList.end()) {
			delete *i;
			i++;
		}
		_sketchItemList.clear();
	}

	void FSketch::CreateSketchItem(string type, string name, ifstream &is)
	{
		FSKETCHItem * pSketchItem = 0;

		if (type == "Point2D") //Point2D : 0
			pSketchItem = new FSKETCHCreate2DPoint(this, 0, name);
		else if (type == "Line2D") //Line2D : 1
			pSketchItem = new FSKETCHCreate2DLine2Points(this, 1, name);
		else if (type == "Circle2D") //Circle2D : 2
			pSketchItem = new FSKETCHCreate2DCircleCenterPoint(this, 2, name);
		else if (type == "Factory2D")
			_factoryName = name;
		else if (type == "GeometricElement")
			_axisName = name;
		else if (type == "CATBaseDispatch")
		{
			if (name.find("point") == 0)
				pSketchItem = new FSKETCHCreate2DPoint(this, 0, name);
			else if (name.find("line") == 0)
				pSketchItem = new FSKETCHCreate2DLine2Points(this, 1, name);
		}

		if (pSketchItem)
		{
			pSketchItem->GetInfo(is);
			_sketchItemList.push_back(pSketchItem);
		}
	}

	void FSketch::GetInfo(ifstream & is)
	{
		char buffer[500];
		char seps_temp[] = " ()="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		string token, name, type;

		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set�� ã�� �κ�
		{
			is.getline(buffer, 500);
		}

		token = strtok_s(buffer, seps_temp, &context);
		token = strtok_s(NULL, seps_temp, &context);
		token = strtok_s(NULL, seps_temp, &context);

		if (token == "sketches1.Add")
		{
			token = strtok_s(NULL, seps_temp, &context);

			// ����ġ�� ���� reference �̸��� ����
			_pReferenceEntity = _pPart->GetReferenceEntityByName(token);

			// SketchManager�� �ڵ����Ʈ�� �����ϴ� �κ�
			// Dim arrayOfVariantOfDouble1(8)�� ã�� �κ�
			while (strncmp(buffer, "Dim", 3))
			{
				is.getline(buffer, 500);
			}

			for (int i = 0; i < 9; ++i)
			{
				is.getline(buffer, 500);

				token = strtok_s(buffer, seps_temp, &context);	//ù��° �ܾ�
				token = strtok_s(NULL, seps_temp, &context);		//�ι�° �ܾ�
				token = strtok_s(NULL, seps_temp, &context);		//����° �ܾ�

				sketchCoord[i] = atof(token.c_str());
			}

			// Sketch ������ �ҷ����� �Լ� ȣ��
			GetSketchItemInfo(is);
		}

		context = NULL;
	}

	void FSketch::GetSketchItemInfo(ifstream & is)
	{
		char buffer[500] = "";
		char seps_temp[] = " ()="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		string token, name, type;

		string nameTemp = _name + ".CloseEdition"; //sketch1.CloseEdition�� ���ڼ��� �����´�.
		int nameSize = nameTemp.size();

		//sketch1.CloseEdition�̶� ������ ���� ������ ����Ƽ�� �о ����
		while (strncmp(buffer, nameTemp.c_str(), nameSize))
		{
			if (strlen(buffer) == 0)  // �� ������ ���� ���
			{
			}
			else if (!strncmp(buffer, "Dim", 3))  //ù��° �ܾ dim���� �˻��Ͽ� ������ ��ü ����
			{
				token = strtok_s(buffer, seps_temp, &context);//Dim
				token = strtok_s(NULL, seps_temp, &context);	//line2D4
				name = token; //��ü�� �̸� ����

				token = strtok_s(NULL, seps_temp, &context);	//As
				token = strtok_s(NULL, seps_temp, &context);	//Line2D
				type = token; //��ü�� Ÿ�� ����

				CreateSketchItem(type, name, is); // ����ġ ������ ����
			}
			else
			{
				//�Ʒ��� ������ ���� Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� ���
				// line2D7.StartPoint = point2D5
				// line2D7.EndPoint = point2D6
				// line2D7.ReportName = 8
				// sketch1.CenterLine = line2D7

				char temp[500];
				char seps_temp2[] = " .()="; //������
				strcpy_s(temp, buffer);

				token = strtok_s(buffer, seps_temp2, &context);
				name = token;

				if (_name == name)  //sketch�� �̸��� ���� ���
				{
					token = strtok_s(NULL, seps_temp2, &context);

					if (token == "CenterLine") //sketch1.CenterLine�� ���� ��� ó��
					{
						token = strtok_s(NULL, seps_temp2, &context);
						GetSketchItemFromName(token)->SetCenterLineInfo(1); //�ش� line�� ���Ͷ������� ����
					}
					else
					{
					}
				}
				else if (GetSketchItemFromName(name)) //SketchItem�� �ִ��� Ȯ��
				{
					GetSketchItemFromName(name)->Modify(temp);  //���� ����
				}
			}

			context = NULL;

			is.getline(buffer, 500);  //���� ������ �б�

		} // end of while

		context = NULL;
	}

	void FSketch::ToTransCAD()
	{
		// Sketch�� ���� ���۷��� ����
		GetReferenceEntity()->ToTransCAD();

		// Add new StdSketchFeature to TransCAD
		_spSketch = GetPart()->_spFeatures->AddNewSketchFeature(GetFeatureName().c_str(), GetReferenceEntity()->GetReferencePtr());

		// Set Coordinate system of Sketch
		_spSketch->SetCoordinateSystem(sketchCoord[0], sketchCoord[1], sketchCoord[2], sketchCoord[3], sketchCoord[4], sketchCoord[5], sketchCoord[6], sketchCoord[7], sketchCoord[8]);

		// TransCAD Sketch Open
		_spSketEditor = _spSketch->OpenEditorEx(VARIANT_FALSE);

		// ����ġ ��� ���� TransCAD�� ��ȯ�ϱ�
		for (int i = 0; (unsigned int)i < GetSketchItemsSize(); ++i)
		{
			GetSketchItem(i)->ToTransCAD();
		}

		// TransCAD Sketch Close
		_spSketEditor->Close();
	}

	FSKETCHItem * FSketch::GetSketchItemFromName(string name)
	{
		vector<FSKETCHItem *>::iterator iter = _sketchItemList.begin();

		while (iter != _sketchItemList.end())
		{
			if (((*iter)->GetSketchItemName()) == name)
				return *iter;

			iter++;
		}

		return 0;
	}

	void FSketch::Modify(char * buffer)
	{

	}

	//SketchItem�� �̸���, �ش� SketchItem�� ReportName�� �����ϱ� ���� �뵵. 
	void FSketch::AddEntityNameReportName(int reportName, string entityName)
	{
		ReportItem _repItem;
		_repItem._reportName = reportName;
		_repItem._entityName = entityName;

		reportNameTable.push_back(_repItem);
	}

	//ReportName�� �Է��Ͽ� �ش� SketchItem�� �̸��� ����ϱ� ���� �뵵.
	string FSketch::GetEntityNameFromReportName(int reportName)
	{
		string entityName;
		for (int i = 0; (unsigned int)i < reportNameTable.size(); ++i)
		{
			if (reportNameTable[i]._reportName == reportName)
				entityName = reportNameTable[i]._entityName;
		}

		return entityName;
	}

}