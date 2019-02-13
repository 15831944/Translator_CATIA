#include "stdafx.h"
#include "AttAngularRepartation.h"

namespace Pre {
	AttAngularRepartation::AttAngularRepartation(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttAngularRepartation::~AttAngularRepartation()
	{
	}

	void AttAngularRepartation::GetInfo(ifstream &is)
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

		LengthValue = strtok_s(buffer, seps_temp, &context);	//ù��° �ܾ�
		LengthValue = strtok_s(NULL, seps_temp, &context);	//�ι�° �ܾ�

		LengthValue = strtok_s(NULL, seps_temp, &context);	//����° �ܾ� : circPattern1
		_featureName = LengthValue;

		LengthValue = strtok_s(NULL, seps_temp, &context);	//�׹�° �ܾ� : AngularRepartition
		_attributeName = LengthValue;

		context = NULL;
	}

	void AttAngularRepartation::Modify(char * buffer)  //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
	}

	void AttAngularRepartation::CheckAttribute(string name, double value, int type)
	{
		if (name == "InstancesCount")
			type = 0;
		else if (name == "AngularSpacing")
			type = 1;

		GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, value, type);
	}
}