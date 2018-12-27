#include "stdafx.h"
#include "AttIntParam.h"
namespace Pre {
	AttIntParam::AttIntParam(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttIntParam::~AttIntParam()
	{
	}

	void AttIntParam::GetInfo(ifstream &is)
	{
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))
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

	void AttIntParam::Modify(char * buffer)  //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
		string token;
		char seps[] = " ,\t\n().=";
		char * context = NULL;

		token = strtok_s(buffer, seps, &context);
		token = strtok_s(NULL, seps, &context);

		if (token == "Value")
		{
			char seps2[] = " ,\t\n()=";
			token = strtok_s(NULL, seps2, &context); //5.700000

			_value = atof(token.c_str());
		}

		//Set intParam1 = linearRepartition1.InstancesCount �� ���� ���. 
		//�� attribute(e.g. linearRepartition1)�� sub-attribute(e.g. InstancesCount)�� ����.
		if (GetPart()->GetAttributeItemByName(_featureName))
		{
			GetPart()->GetAttributeItemByName(_featureName)->CheckAttribute(_attributeName, _value, _type);
		}

		context = NULL;
	}
}