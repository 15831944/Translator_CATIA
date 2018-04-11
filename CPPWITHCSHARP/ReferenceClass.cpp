
#include "stdafx.h"
#include "ReferenceClass.h"
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;

string Stos(String^ k) { return  msclr::interop::marshal_as< std::string >(k); }
String^ stoS(string k) { return  msclr::interop::marshal_as<String^>(k); }

/** Post Process Reference ����
**************** Puclic
* //�ϳ��� Constraint�� �ϱ� ���� Master Part�� Slave Part�� ���� ReferenceName�� ����
* // TransCAD Assembly���� ����ϴ� Part����.CATScript�� ��ȯ�ϱ� ���� �Լ�
**************** Private
* //�ϳ��� Component�� �޾ƿͼ� Part�̸��� ���� �� Component�� ���° Part���� ��ȣ�� ���� 
* //�ϳ��� Assem�� �޾ƿͼ� SubAssembly�̸��� ���Ͽ� ���° CATIA�� Product�� �ش��ϴ��� �˾Ƴ�
* //Type�� ���ڷ� �޾Ƽ� CATIA���� ����ϴ� consist Name���� �ٲ���
* //Part ������ ���ؼ� TransCAD ReferenceName�� CATIA ReferenceName���� ��ȯ
* //TransCAD Assembly�� Reference �̸��� CATIA Reference �̸����� ����
**/
namespace ReferenceClass
{
	
	//������ C++ �ڵ� Dll ��������ִ� ��
	ref_Post::ref_Post() :m_pReferenceManager(new CATIAReferenceManager) {	}

	//�Ҹ���
	ref_Post::~ref_Post() { delete m_pReferenceManager;	m_pReferenceManager = 0; }


	/**********************************�Ʒ����ʹ� Public �Լ���***********************************************/


	//�ϳ��� Constraint�� �ϱ� ���� Master Part�� Slave Part�� ���� ReferenceName�� ����
	refCommand ref_Post::ConvertRefPost(refCommand w) {		   //w.command : Constraint Type(int), 
															   //w.Param : Reference Pack;
															   //w.MasterPart : Master Part Name
															   //w.SlavePart : Slave Part Name
															   //w.MasterPartRef : Master Part Reference Name
															   //w.SlavePartRef : Slave Part Reference Name
		#pragma region Code _ Initialize For Get Parts Information

		//dll�� ���ؼ� Part Document�� ����
		TransCAD::IApplicationPtr g_spApplication;
		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;
		//_spAssem �Ʒ��� _spComp(SubAssembly����=Parts����)��� _spConstraints(Constraint����)�� ����ִ�
		//_spComp(product) �Ʒ��� Part������ ��� �ִ�
		
		
		//COM initialize
		int status = false;
		HRESULT hr = ::CoInitialize(NULL);
		//TransCAD instance ����
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TransCAD initialize error check
		if (FAILED(hr))status = 0;
		//Assembly document ����
		_spAssemDocument = g_spApplication->ActiveDocument;
		//Assem ��ü ����
		_spAssem = _spAssemDocument->GetAssem();

		#pragma endregion



		#pragma region Code _ Translate Reference Name TransCAD to CATIA
		//Translate MasterPartReferenceName
		string master = T2C_ReferenceName_From_Assembly(_spAssem, w.command, Stos(w.param), Stos(w.MasterPart), Stos(w.MasterPart_Ref));

		//Translate SlavePartReferenceName
		string slave = T2C_ReferenceName_From_Assembly(_spAssem, w.command, Stos(w.param), Stos(w.SlavePart), Stos(w.SlavePart_Ref));
		#pragma endregion



		#pragma region Code _ Get Result And Return
		::CoUninitialize();						// COM Automation API Uninitialization

		refCommand result;
		result.command = w.command;              //Constraint Type id
		result.param = GetConstType(w.command);  //constraint type name
		result.MasterPart = w.MasterPart;        //Master Part
		result.SlavePart = w.SlavePart;          //Slave Part
		result.MasterPart_Ref = stoS(master);    //Master Reference
		result.SlavePart_Ref = stoS(slave);      //Slave Reference
		#pragma endregion
		return result;
	}

	//TransCAD Assembly���� ����ϴ� Part���� .CATScript�� ��ȯ�ϱ� ���� �Լ�
	void ref_Post::ExtentionFuc(int CompNum,int PartNum,String^ path) {
	
		//dll�� ���ؼ� Part Document�� ����
		TransCAD::IApplicationPtr g_spApplication;
		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;
		//_spAssem �Ʒ��� _spComp(SubAssembly����=Parts����)��� _spConstraints(Constraint����)�� ����ִ�
		//_spComp(product) �Ʒ��� Part������ ��� �ִ�


		//COM initialize
		int status = false;
		HRESULT hr = ::CoInitialize(NULL);
		//TransCAD instance ����
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TransCAD initialize error check
		if (FAILED(hr))status = 0;
		//Assembly document ����
		_spAssemDocument = g_spApplication->ActiveDocument;
		//Assem ��ü ����
		_spAssem = _spAssemDocument->GetAssem();
		
		TransCAD::ICompPtr _spComp = _spAssem->GetComponent(CompNum);
		TransCAD::IPartPtr part = _spComp->GetPart(PartNum);

		g_pRefManager = new CATIAReferenceManager();
		
		Part *exPart;
		exPart = new Part(part,Stos(path));
		exPart->ExternModeToggle(0);//0�̸� ���������� Part�� GetInfo �Ѵ�
		                            //1�̸� Assembly�� �ϱ����� Part�� GetInfo�ϴ� ���� �ٲ�
		exPart->GetInfo();
		exPart->ToCATIA();
		exPart->InitReferManager(); //��Ʈ�� ��ȯ�Ͽ����� ����ߴ� �������� ��� �ٽ� �ʱ�ȭ�Ѵ�(���� ��Ʈ�鵵 ��ȯ�ϱ� ���ؼ�...)
		delete exPart;
		//::CoUninitialize();						// COM Automation API Uninitialization
		//g_pRefManager->~CATIAReferenceManager();
				
	
	}






	/**************************************** ������ʹ� Private �Լ��� ********************************************/
	//�ϳ��� Component�� �޾ƿͼ� Part�̸��� ���� �� Component�� ���° Part���� ��ȣ�� ����  //�ٽ� �����ߵ� Assem�� �޾ƿͼ� ���° Component������ ���� PartNum�� +�ǾߵǴ� ���� �ִ�//����� �׳� �ϳ��� ���۳�Ʈ �ȿ��� ���° ��Ʈ������ �����Ѵ�//��ü ��Ʈ���� ��� ° ��Ʈ������ �������� �ʴ´�
	int ref_Post::GetPartNum_From_PartName(TransCAD::ICompPtr u_spComp, string PartName) {

		int partNum = 0;

		int partCnt = u_spComp->GetSize();
		//string a = (string)u_spComp->get_Name();
		for (int i = 0; i < partCnt; ++i) {
			string partName;
			TransCAD::IPartPtr part = u_spComp->GetPart(i); //cout << "Name of part [" << i << "] in component1" << part->Name << endl;
			if ((string)(part->Name) == PartName) {
				partNum = i;
				break;
			}
		}
		return partNum;


	}

	//�ϳ��� Assem�� �޾ƿͼ� SubAssembly�̸��� ���Ͽ� ���° CATIA�� Product�� �ش��ϴ��� �˾Ƴ�
	int ref_Post::GetProductNum_From_SubAssemName(TransCAD::IAssemPtr u_spAssem, string SubAssemName)
	{

		int SuAssemNum = 0;

		int assemCnt = u_spAssem->GetSize();
		//string a = (string)u_spComp->get_Name();
		for (int i = 0; i < assemCnt; ++i) {
			string partName;
			TransCAD::ICompPtr subassem = u_spAssem->GetComponent(i); //cout << "Name of part [" << i << "] in component1" << part->Name << endl;
			if ((string)(subassem->get_Name()) == SubAssemName) {
				SuAssemNum = i;
				break;
			}
		}
		return SuAssemNum;


		return 0;
	}

	//Type�� ���ڷ� �޾Ƽ� CATIA���� ����ϴ� consist Name���� �ٲ���
	String^ ref_Post::GetConstType(int type) {
		switch (type) {
		case 0:
			return "catCstTypeOn";
			break;
		case 1:
			return "catCstTypeSurfContact";
			break;
		default:
			return "ANY";
			break;



		}

	}

	//Part ������ ���ؼ� TransCAD ReferenceName�� CATIA ReferenceName���� ��ȯ
	string ref_Post::T2C_ReferenceName_From_Part(TransCAD::ICompPtr u_spComp, int partNum, string part_persistentName) {

		string partName;

		TransCAD::IPartPtr part = u_spComp->GetPart(partNum);

		g_pRefManager = new CATIAReferenceManager();
		Part *exPart;
		exPart = new Part(part, 0);
		exPart->ExternModeToggle(1);
		exPart->GetInfo();
		std::string parsing = g_pRefManager->ConvertToBRepFace(part_persistentName);
		cout << "ChangedName : [ " << parsing << "]" << endl;
		exPart->InitReferManager();
		//delete exPart;



		return parsing;

	}

	//TransCAD Assembly�� Reference �̸��� CATIA Reference �̸����� ����
	string ref_Post::T2C_ReferenceName_From_Assembly(TransCAD::IAssemPtr u_spAssem, int constype, string Pack, string PartName, string assembly_persistentName) {
		//u_spComp(SubAssembly1���� Parts�鿡 ���� ������ ��� ����) �ȿ� �ִ� PartName�� �˻��ؼ� assembly_persistentName�� T2C�� ��ȯ�ϴ� �۾�

		string temp = assembly_persistentName;											  //SubAssembly1,RotationPart,Cut1,Sketch2,Circle1,0,0,0,ExtrudeFeature:0,0:0:0

		string refer_product = temp.substr(0, temp.find(",") - 0);						  //SubAssembly1 ����    C#::string refer_product = refer.Substring(0, refer.IndexOf(","));
		int SubAssemNum = GetProductNum_From_SubAssemName(u_spAssem, refer_product);      //Product Number ��������
		refer_product = "Product" + to_string(SubAssemNum);								  //Product1 ��ȯ        C#::refer_product = refer_product.Substring(11);refer_product = "Product" + refer_product;

		TransCAD::ICompPtr u_spComp = u_spAssem->GetComponent(SubAssemNum);
		int partNum = GetPartNum_From_PartName(u_spComp, PartName);						  //PartNum ��������
		string refer_Part = "Part1." + to_string(partNum + 1);							  //Part1.1(Part�̸�.�Ȱ���Part�̸��켱����)          C#::string refer_Part = "Part1." + (partNum + 1).ToString();
		string refer_pack = Pack;                                                         //Axis:(           C#::string refer_coaxial = "Axis" + ":(";
		string refer_from_part = temp.substr(temp.find(",", temp.find(",") + 1) + 1);     //Cut1,Sketch2,Circle1,0,0,0,ExtrudeFeature:0,0:0:0  C#::string refer_from_part = refer.Substring(refer.IndexOf(",") + 1);refer_from_part = refer_from_part.Substring(refer_from_part.IndexOf(",") + 1);refer_from_part = m_refer.EX(m_param);
		refer_from_part = T2C_ReferenceName_From_Part(u_spComp, partNum, refer_from_part);


		string result = "\"" + refer_product + "/" + refer_Part + "/!" + refer_pack + refer_from_part + ")\"";

		return result;

	}



	
}



/** Pre Process Reference ����
*
*
**/
namespace ReferenceClass
{
	//������ C++ �ڵ� Dll ��������ִ� ��
	ref_Pre::ref_Pre() {	

		TestEnvironment();

		//COM Initialize
		int status = false;
		HRESULT hr = CoInitialize(NULL);
		hr = Pre::g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		if (FAILED(hr))status = 0;
		if (!Pre::g_spApplication->Visible) { Pre::g_spApplication->Visible = VARIANT_TRUE; }

		_spAssemDocument = new TransCAD::IAssemDocumentPtr();
		_spAssem = new TransCAD::IAssemPtr();
	
		
		//���� Assem Document ��������
		//TransCAD::IAssemDocumentPtr set = Pre::g_spApplication->ActiveDocument;
		*_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		*_spAssem = (*_spAssemDocument)->GetAssem();

		pre_data = new parsing();
	}



	//�Ҹ���
	ref_Pre::~ref_Pre() { ::CoUninitialize(); }

	void ref_Pre::TestEnvironment() {
		//COM Initialize
		int status = false;
		HRESULT hr = CoInitialize(NULL);
		//TransCAD instance ����
		hr = Pre::g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TrnasCAD initialzie error check
		if (FAILED(hr))status = 0;

		//TransCAD ����ȭ
		if (!Pre::g_spApplication->Visible) {

			Pre::g_spApplication->Visible = VARIANT_TRUE;
		}

		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;
		TransCAD::ICompPtr _spComp;
		TransCAD::IPartDocumentPtr _spPartDocument;


		//��Ʈ ���� ����

		char* asfe1 = "D:\\project\\Macro\\TestAssem\\SubAssembly1\\RotationPart.CATScript";
		char* asfe2 = "D:\\project\\Macro\\TestAssem\\SubAssembly1\\Body.CATScript";

		_spPartDocument = Pre::g_spApplication->GetDocuments()->AddPartDocument();
		Pre::Part _spConstraintedPart(asfe1, _spPartDocument);
		_spConstraintedPart.GetInfo();
		_spConstraintedPart.ToTransCAD();

		_spPartDocument = Pre::g_spApplication->GetDocuments()->AddPartDocument();
		Pre::Part _spReferPart(asfe2, _spPartDocument);
		_spReferPart.GetInfo();
		_spReferPart.ToTransCAD();


		//����� ��ť��Ʈ ���� �� ��Ʈ ���� �߰�
		_spAssemDocument = Pre::g_spApplication->GetDocuments()->AddAssemDocument();

		_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		_spAssem = _spAssemDocument->GetAssem();
		_spComp = _spAssem->CreateComponent();//SubAssembly1
		_spComp->set_Name("Component1");

		_spComp->AddPart(_spConstraintedPart._spPart);
		_spComp->AddPart(_spReferPart._spPart);

		_spAssem->AddComponent(_spComp);
		_spAssemDocument->Update();
	
	}

	void ref_Pre::PreTest(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref) {

	
	}

	refCommand ref_Pre::SetConstraint(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref, String^ _option, int^ _option_int) {
																											/*Input ��
																											1 : Total Part Num
																											2 : [Rotation,Body], [Part1,Part1], [D:\\project\\Macro\\TestAssem\\SubAssembly1\\RotationPart.CATScript, "D:\project\Macro\TestAssem\SubAssembly1\Body.CATPart"]
																											(TransCAD�̸�/����_�̸�, CATIA�̸�, CATIA�̸��� ���� ���)
																											3 : catCstTypeOn,
																											4 : "Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))"
																											5 : "Product1/Part1.2/!Axis:(Selection_RSur:(Face:(Brp:(Pad.2;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pad.2_ResultOUT;Z0;G3563))"
																											*/

		//Step1 : �ʱ�ȭ - TrnasCAD �� Pre ���� �ʱ�ȭ
#pragma region Step1 : �ʱ�ȭ - TrnasCAD �� Pre ���� �ʱ�ȭ

		string CstType = Stos(_CstType);
		string full_ref[(int)TARGETPART::REFERENCE_SIZE] = { Stos(_master_ref), Stos(_slave_ref)};

		//���� Assem Document ��������
		//Pre::g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		*_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		*_spAssem = (*_spAssemDocument)->GetAssem();


#pragma endregion 
 
#pragma region Step2 :parsing & Translate

			

		TransCAD::ICompPtr _IspComp;
		TransCAD::IPartPtr TargetPart[(int)TARGETPART::REFERENCE_SIZE]; //MASTER,SLAVE
		int index[(int)TARGETPART::REFERENCE_SIZE];
		string trans_ref[(int)TARGETPART::REFERENCE_SIZE];

		


		/********Step2_1 : cn = 0 : Master Part�� Master_geometry ã��********/
		/********Step2_1 : cn = 1 : Slave Part�� Slave_geometry ã��********/
		for (int cn = 0; cn < 2; cn++) {

			Parsing(stoS(full_ref[cn]), &pre_data->assem_product, &pre_data->assem_part, &pre_data->assem_geometry);
			index[cn] = GetTransCADName_from_buffer(buffer, &pre_data->Transcad_subAssemName, &pre_data->Transcad_partName, &pre_data->Transcad_geometry);
			trans_ref[cn] = pre_data->Transcad_geometry;
			
			//transcad name �� ���� TransCAD Part �ν��Ͻ��� ������
			_IspComp = (*_spAssem)->GetComponent(pre_data->product_num);
			for (int i = 0; i < _IspComp->GetSize(); i++) {
				if ((string)(_IspComp->GetPart(i)->Name) == pre_data->Transcad_partName) {
					TargetPart[cn] = _IspComp->GetPart(i);
					break;
				}
			}
		
		}


#pragma endregion

#pragma region Step3 : TransCAD�� Constraint ����
		//Constraints�����ڸ� ��������, //�� Contratint�� Master/Slave Part�� Reference Name�� ������ ReferencePtr�� ���� 
		TransCAD::IStdAssemConstraintsPtr _spConstraints;
		_spConstraints = (*_spAssem)->GetConstraints();//Constraint ������

		TransCAD::IReferencePtr _spConstrainedGeometry, _spReferGeometry;

		//�� Constraint�� Master Part�� Reference Name�� _spConstrainedGeometry�� ����
		//�� Constraint�� Slave Part�� Reference Name�� _spReferGeometry�� ����
		_spConstrainedGeometry = TargetPart[(int)TARGETPART::MASTER]->SelectBrepByName(trans_ref[(int)TARGETPART::MASTER].c_str());
		_spReferGeometry = TargetPart[(int)TARGETPART::SLAVE]->SelectBrepByName(trans_ref[(int)TARGETPART::SLAVE].c_str());

		//Constraint�� �ϱ� ���� Constraint �����ڿ��� Constraint ������ �־� �ش�
		//"Constraint Type", "Master Part", "Master Reference Name", "Slave Part", "Slave ReferenceName"

		//_spConstraints->AddNewAssemblyCoaxialConstraint("Coaxial", MasterPart, _spConstrainedGeometry, SlavePart, _spReferGeometry);
		//or AddNewAssemblyIncidenceConstraint "Incidence"

		//_spConstraints->AddNewAssemblyIncidenceConstraint("Incidence", TargetPart[(int)TARGETPART::MASTER], _spConstrainedGeometry, TargetPart[(int)TARGETPART::SLAVE], _spReferGeometry, TransCAD::StdAssemblyIncidenceType::Opposite);
		string fjeiakso = trans_ref[(int)TARGETPART::MASTER];
		cout << fjeiakso << endl;
		fjeiakso = trans_ref[(int)TARGETPART::SLAVE];
		cout << fjeiakso << endl;
		string param = Constraint_to_TransCAD(_spConstraints, CstType, TargetPart[(int)TARGETPART::MASTER], _spConstrainedGeometry, TargetPart[(int)TARGETPART::SLAVE], _spReferGeometry,Stos(_option),_option_int);
		
		
		(*_spAssemDocument)->Update();
#pragma endregion

#pragma region Step3 : ��� ��������
		refCommand result;
		result.command = TRUE;
		result.param = stoS(param);
		result.MasterPart = buffer->Getitem_from_index(2, index[(int)TARGETPART::MASTER]);
		result.MasterPart_Ref = stoS(trans_ref[(int)TARGETPART::MASTER]);
		result.SlavePart = buffer->Getitem_from_index(2, index[(int)TARGETPART::SLAVE]);
		result.SlavePart_Ref = stoS(trans_ref[(int)TARGETPART::SLAVE]);

		cout << "Contraint Translate Result" << endl;
		cout << Stos(result.MasterPart) << " : " << Stos(result.MasterPart_Ref) << endl;
		cout << Stos(result.SlavePart) << " : " << Stos(result.SlavePart_Ref) << endl;


#pragma endregion
		

		

		return result;
	}

	void ref_Pre::Parsing(String^ ref, string* product, string* part, string* geometry) {

		

		string temp = Stos(ref);
		string assem_product = temp.substr(0, temp.find('/'));

		temp = temp.substr(temp.find('/') + 1, temp.size());
		string assem_part = temp.substr(0, temp.find('/'));

		temp = temp.substr(temp.find('/') + 1, temp.size());
		string assem_geometry = temp.substr(0, temp.find('/'));
		assem_geometry = "\"" + assem_geometry.substr(assem_geometry.find('!') + 1, assem_geometry.size()) + "\"";


		*product = assem_product; 
		*part = assem_part; 
		*geometry = assem_geometry;
	}

	int ref_Pre::GetTransCADName_from_buffer(PreStack^ buffer,string* Transcad_subAssemName,string* Transcad_partName,string* Transcad_geometry) {

		TransCAD::ICompPtr _spComp;
		string product = pre_data->assem_product;
		string part = pre_data->assem_part;
		string geo = pre_data->assem_geometry;

		//Step1 : Product1 �� ���ؼ� TransCAD�� Component(SubAssembly) Name�� Number�� ������
		int comp_num = stoi(product.substr(product.find("t") + 1, product.size()));//Product ���� Parsing
		_spComp = (*_spAssem)->GetComponent(comp_num);
		*Transcad_subAssemName = (string)_spComp->get_Name();
		

		//Step2-1 : Part1.1 �� ���ؼ� Part1 �� Part1�� Number�� ������
		int part_num = stoi(part.substr(part.find(".") + 1, part.size()));
		part = part.substr(0, part.find("."));
		//Step2-2 : buffer���� Number ��° Part1�� index�� ������
		int buffer_index = buffer->Getindex_from_name(stoS(part), part_num);
		//Step2-3 : buffer���� index�� �ش��ϴ� transcad name�� ������
		*Transcad_partName = Stos(buffer->Getitem_from_index(2, buffer_index));


		//Step3 : CATScript�κ��� Geometry�� ��ȯ
		 string _path = Stos(buffer->Getitem_from_index(0, buffer_index));
		_path = _path.substr(0, _path.find_last_of('.')) + ".CATScript";

		Pre::Part* pPart = new Pre::Part(_path, 1);
		pPart->GetInfo();															//Part ���� �б�
		Pre::ReferenceEntity* _refer = new Pre::ReferenceEntity(pPart, 0, "a");		//Part���� Feature���� ����

		if (geo.substr(1, 6) == "Axis:(") { geo  = "\"" + geo.substr(7, geo.size()); }//���� ������ ���̶�� �� �պκ� Axis ����
		char* ptr = (char*)geo.c_str();
		*Transcad_geometry = *Transcad_subAssemName + "," + *Transcad_partName + "," + _refer->GetReferName(ptr);

		delete _refer;
		//delete pPart;

		pre_data->product_num = comp_num;
		pre_data->part_num = part_num;

		

		return buffer_index;
	}

	string ref_Pre::Constraint_to_TransCAD(TransCAD::IStdAssemConstraintsPtr f,string type, TransCAD::IPartPtr master,TransCAD::IReferencePtr master_ref, TransCAD::IPartPtr slave, TransCAD::IReferencePtr slave_ref,string option, int^ option_int ) {
		string re;
		if (option == "move") {
			TransCAD::ICompPtr _lomp = (*_spAssem)->GetComponent(pre_data->product_num);
			_lomp->SetPartPlacement(master, 10, 100, 100, 0, 0, 1, 1, 0, 0);
			(*_spAssemDocument)->Update();
		}
		

		if (type == "catCstTypeOn") {

			f->AddNewAssemblyCoaxialConstraint("Coaxial", master, master_ref, slave, slave_ref);
			re = "Coaxial";

		}
		else if (type == "catCstTypeSurfContact") {
			if (option == "same") {
				f->AddNewAssemblyIncidenceConstraint("Incidence", master, master_ref, slave, slave_ref, TransCAD::StdAssemblyIncidenceType::Same);
			}
			else {
			
				f->AddNewAssemblyIncidenceConstraint("Incidence", master, master_ref, slave, slave_ref, TransCAD::StdAssemblyIncidenceType::Opposite);
			}
			
			re = "Incidence";
			
		}
		else {
			re =  "None";
		
		}

		return re;
		
	}

}