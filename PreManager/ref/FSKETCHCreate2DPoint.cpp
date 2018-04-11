#include "stdafx.h"
#include "FSKETCHCreate2DPoint.h"
#include "FSketch.h"

namespace Pre {
	FSKETCHCreate2DPoint::FSKETCHCreate2DPoint(FSketch * pFSketch, int type, string name)
		: FSKETCHItem(pFSketch, type, name)
	{
	}

	FSKETCHCreate2DPoint::~FSKETCHCreate2DPoint(void)
	{
	}

	void FSKETCHCreate2DPoint::GetInfo(ifstream & is)
	{
		char buffer[500];
		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set constraint9�� ã�� �κ�
		{
			is.getline(buffer, 500);
		}

		string point_info;
		char seps_temp[] = " ,\"\t\n()="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		point_info = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ� Set
		point_info = strtok_s(NULL, seps_temp, &context);  //point2D1
		point_info = strtok_s(NULL, seps_temp, &context);  //factory2D1.CreatePoint

		point_info = strtok_s(NULL, seps_temp, &context);  //100.000000

		if (point_info == "Origin" || point_info == "����")
		{
			//_point[0] = GetFeatureSketch()->sketchCoord[0];
			//_point[1] = GetFeatureSketch()->sketchCoord[1];
			//_point[2] = GetFeatureSketch()->sketchCoord[2];
		}
		else
		{
			_point[0] = atof(point_info.c_str());

			point_info = strtok_s(NULL, seps_temp, &context);  //0.000000
			_point[1] = atof(point_info.c_str());

			_point[2] = 0.000000;
		}
	}

	void FSKETCHCreate2DPoint::ToTransCAD()
	{
		// Line ���� �� �� �ڵ����� �����ǹǷ� �ʿ䰡 ����.
	}

	void FSKETCHCreate2DPoint::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		token = strtok_s(buffer, seps, &context);  //line2D7
		token = strtok_s(NULL, seps, &context); //StartPoint

		if (token == "ReportName")
		{
			token = strtok_s(NULL, seps, &context); //point2D3

			_id = atoi(token.c_str());
			GetFeatureSketch()->AddEntityNameReportName(_id, _name);
		}
		else
		{
		}
	}
}