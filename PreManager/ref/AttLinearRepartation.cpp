#include "stdafx.h"
#include "AttLinearRepartation.h"
namespace Pre {

	AttLinearRepartation::AttLinearRepartation(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttLinearRepartation::~AttLinearRepartation()
	{
	}

	void AttLinearRepartation::GetInfo(ifstream &is)
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
		LengthValue = strtok_s(NULL, seps_temp, &context);	//�ι�° �ܾ�

		LengthValue = strtok_s(NULL, seps_temp, &context);	//����° �ܾ� : rectPattern1
		_featureName = LengthValue;

		LengthValue = strtok_s(NULL, seps_temp, &context);	//�׹�° �ܾ� : FirstDirectionRepartition
		_attributeName = LengthValue;

		context = NULL;
	}

	void AttLinearRepartation::Modify(char * buffer)  //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
	}

	void AttLinearRepartation::CheckAttribute(string name, double value, int type)
	{
		if (name == "InstancesCount")
			type = 0;
		else if (name == "Spacing")
			type = 1;

		GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, value, type);
	}
}