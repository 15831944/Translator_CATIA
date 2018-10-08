#pragma once
#include "ReferenceClass.h"

namespace ReferenceClass
{
	public delegate void SelectedDelegateCSharp(Object ^j);//For c# Raise
	public delegate void SelectedDelegateCplus(int i); //for C++ Raise

	public ref class ref_Post
	{


	public:
		ref_Post();
		virtual ~ref_Post();

	protected:
		Post::CATIAReferenceManager	*m_pReferenceManager;

	private:
		int GetPartNum_From_PartName(TransCAD::ICompPtr u_spComp, string PartName);
		int GetProductNum_From_SubAssemName(TransCAD::IAssemPtr u_spAssem, string SubAssemName);
		string T2C_ReferenceName_From_Part(TransCAD::ICompPtr u_spComp, int partNum, string persistentName);
		string T2C_ReferenceName_From_Assembly(TransCAD::IAssemPtr u_spAssem, int constype, string Pack, string PartName, string assembly_persistentName);
		String^ GetConstType(int type);
	public:
		////////////������ ������ �޼��� �߰�//////////////
		/*�ܺη� �����ų �Լ��� ����� �� �ȿ��� m_pReferenceManager�� ���� ���� �Լ��� ����Ѵ�*/
		//::Part * pPart;
		refCommand ConvertRefPost(refCommand w);
		void AssemblyPartT2C(int CompNum, int PartNum, String^ path);

	};



}