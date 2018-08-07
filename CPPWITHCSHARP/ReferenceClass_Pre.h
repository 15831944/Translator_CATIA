#pragma once
#include "ReferenceClass.h"

namespace ReferenceClass
{




	public ref class ref_Pre
	{
		enum class TARGETPART : int { MASTER, SLAVE, REFERENCE_SIZE };//REFERENCE_SIZE�� �׻� �ǵڿ� �־���Ѵ�.

	public:
		ref_Pre();
		ref_Pre(PreStack^ buffer);
		virtual ~ref_Pre();

	private:
		//::CATIAReferenceManager	*m_pReferenceManager;
	public:
		TransCAD::IAssemDocumentPtr* _spAssemDocument;
		TransCAD::IAssemPtr* _spAssem;
		TransCAD::ICompPtr* _spComp;

		parsing* pre_data;

	public:
		////////////������ ������ �޼��� �߰�//////////////
		/*�ܺη� �����ų �Լ��� ����� �� �ȿ��� m_pReferenceManager�� ���� ���� �Լ��� ����Ѵ�*/
		void PreTest(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref);
		void TestEnvironment(PreStack^ buffer);
		refCommand ref_Pre::SetConstraint(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref, String^ _option, int^ _option_int);

		void Parsing(String^ ref, string* product, string* part, string* geometry);
		int GetTransCADName_from_buffer(PreStack^ buffer, string* Transcad_subAssemName, string* Transcad_partName, string* Transcad_geometry);
		string Constraint_to_TransCAD(TransCAD::IStdAssemConstraintsPtr f, string type, TransCAD::IPartPtr master, TransCAD::IReferencePtr master_ref, TransCAD::IPartPtr slave, TransCAD::IReferencePtr slave_ref, string option, int^ option_int);

	};
}