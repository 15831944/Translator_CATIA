#include "stdafx.h"
#include "AttAngle.h"

namespace Pre {
	AttAngle::AttAngle(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttAngle::~AttAngle()
	{
	}

	void AttAngle::GetInfo(ifstream &is)
	{
		// Angle�� ���� �Է�
		char buffer[500];

		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set angle1�� ã�� �κ�
		{
			is.getline(buffer, 500);
		}

		string AngleValue;
		char seps_temp[] = " ,\t\n()=."; //������
		char * context = NULL;

		AngleValue = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ�
		AngleValue = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�

		AngleValue = strtok_s(NULL, seps_temp, &context);  //����° �ܾ� : _featureName, shaft1
		_featureName = AngleValue;

		AngleValue = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ� : _attributeName, SecondAngle
		_attributeName = AngleValue;

		context = NULL;
	}

	void AttAngle::Modify(char * buffer)  //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
		string token;
		char seps[] = " ,\t\n().="; //������
		char * context = NULL;

		token = strtok_s(buffer, seps, &context);  //length2
		token = strtok_s(NULL, seps, &context); //�ι�° ������

		if (token == "Value")
		{
			char seps2[] = " ,\t\n()="; //������
			token = strtok_s(NULL, seps2, &context); //5.700000

			_value = atof(token.c_str());
		}

		if (GetPart()->GetFeatureByName(_featureName)) //Set length2 = hole1.Diameter �� ���� ���. �� feature(e.g. hole1)�� attribute(e.g. Diameter)�� ����.
		{
			GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, _value, _type);
		}
		else if (GetPart()->GetAttributeItemByName(_featureName)) //Set length9 = limit1.Dimension �� ���� ���. �� attribute(e.g. limit1)�� sub-attribute(e.g. Dimension)�� ����.
		{
			GetPart()->GetAttributeItemByName(_featureName)->CheckAttribute(_attributeName, _value, _type);
		}

		context = NULL;
	}
}