#include "stdafx.h"
#include "Feature.h"

#include <iostream>
#include "Part.h"

using namespace std;

namespace Pre {
	Feature::Feature(Part * pPart, int type, string name)
	{
		_pPart = pPart;									// Set Part ptr
		_type = type;
		_name = name;

		//context = NULL;						// strtok_s �Լ��� ���� �ʱ�ȭ
	}


	Feature::~Feature()
	{
		//context = NULL;						// strtok_s �Լ��� ���� �ʱ�ȭ
	}


	void Feature::CheckAttribute(string name, double value, int type)
	{
	}


	void Feature::Modify(char * buffer)
	{
	}
}