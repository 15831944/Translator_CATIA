#include "stdafx.h"
#include "FSKETCHItem.h"

namespace Pre {
	FSKETCHItem::FSKETCHItem(FSketch * pFSketch, int type, string name)
	{
		_pFSketch = pFSketch;				// Set FSketch Ptr
		_name = name;						// Set Item Name
		_type = type;						// Set Item Type
		_centerLineInfo = 0;				// �Ϲ� Line�� ��� 0, centerLine�� ��� 1. ���߿� ������ �Է�
	}

	FSKETCHItem::~FSKETCHItem(void)
	{
	}

	void FSKETCHItem::GetInfo(ifstream & is)
	{
	}

	void FSKETCHItem::ToTransCAD()
	{
	}

	void FSKETCHItem::Modify(char * buffer)
	{
	}
}