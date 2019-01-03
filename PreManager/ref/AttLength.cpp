#include "stdafx.h"
#include "AttLength.h"

namespace Pre {
	AttLength::AttLength(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttLength::~AttLength()
	{
	}

	void AttLength::GetInfo(ifstream &is)
	{
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))  // Set Length1�� ã�� �κ�
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string LengthValue;
		char seps_temp[] = " ,\t\n()=."; //������
		char * context = NULL;

		LengthValue = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ�
		LengthValue = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�

		LengthValue = strtok_s(NULL, seps_temp, &context);  //����° �ܾ� : _featureName
		_featureName = LengthValue;

		LengthValue = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ� : _attributeName
		_attributeName = LengthValue;

		context = NULL;
	}

	void AttLength::Modify(char * buffer)  //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
		string token;
		char seps[] = " ,\t\n().=";
		char * context = NULL;

		token = strtok_s(buffer, seps, &context);
		token = strtok_s(NULL, seps, &context);

		if (token == "Value")
		{
			char seps2[] = " ,\t\n()=";
			token = strtok_s(NULL, seps2, &context);

			_value = atof(token.c_str());
		}

		if (GetPart()->GetFeatureByName(_featureName)) //Set length2 = hole1.Diameter �� ���� ���. �� feature(e.g. hole1)�� attribute(e.g. Diameter)�� ����.
		{
			GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, _value, (int)0);
		}
		else if (GetPart()->GetAttributeItemByName(_featureName)) //Set length9 = limit1.Dimension �� ���� ���. �� attribute(e.g. limit1)�� sub-attribute(e.g. Dimension)�� ����.
		{
			GetPart()->GetAttributeItemByName(_featureName)->CheckAttribute(_attributeName, _value, (int)0);
		}
	}
}