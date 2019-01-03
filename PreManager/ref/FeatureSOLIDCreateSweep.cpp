#include "stdafx.h"
#include "FeatureSOLIDCreateSweep.h"
#include "ReferenceEntity.h"
namespace Pre {
	FeatureSOLIDCreateSweep::FeatureSOLIDCreateSweep(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		sketch1 = "none";
		sketch2 = "none";
	}

	FeatureSOLIDCreateSweep::~FeatureSOLIDCreateSweep()
	{
	}

	void FeatureSOLIDCreateSweep::GetInfo(ifstream &is)
	{
		//sketch manager�� �̸��� ã�� �� ProtrusionExtrude�� �����ͷ� �Է�
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set rib1�� ã�� �κ�
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string sketchManagerName;
		char seps_temp[] = " ,\t\n()="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		sketchManagerName = strtok_s(buffer, seps_temp, &context); //ù��° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //�ι�° �ܾ�
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //����° �ܾ�

		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //�׹�° �ܾ�, reference13

		// Nothing�� ��쿡�� Modify �Լ����� ó��
		if (sketchManagerName != "Nothing")
		{
			sketch1 = GetPart()->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			sketchManagerName = strtok_s(NULL, seps_temp, &context);  //������° �ܾ�, reference14
			sketch2 = GetPart()->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			// Feature�� Sketch������ �����Ѵ�.
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch1);
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch2);
		}

		context = NULL;
	}

	void FeatureSOLIDCreateSweep::ToTransCAD()
	{
		cerr << "#-- Call ProtrusionSweep::Translate2TransCAD() --#" << endl;

		bstr_t baseSketchName((GetPart()->GetFeatureByName(sketch1)->GetFeatureName().c_str()));
		TransCAD::IReferencePtr spBaseSketch = GetPart()->_spPart->SelectObjectByName(baseSketchName);

		bstr_t guideSketchName((GetPart()->GetFeatureByName(sketch2)->GetFeatureName().c_str()));
		TransCAD::IReferencePtr spGuideSketch = GetPart()->_spPart->SelectObjectByName(guideSketchName);

		if (_type == 4)
			// Create a protrusion sweep feature with the sketches
			GetPart()->_spFeatures->AddNewSolidProtrusionSweepFeature(_name.c_str(), spBaseSketch, spGuideSketch);
		else
			// Create a protrusion sweep feature with the sketches
			GetPart()->_spFeatures->AddNewSolidCutSweepFeature(_name.c_str(), spBaseSketch, spGuideSketch);

		cerr << "#-- End ProtrusionSweep::Translate2TransCAD() --#" << endl << endl;
	}

	void FeatureSOLIDCreateSweep::CheckAttribute(string name, double value, int type)
	{
	}

	void FeatureSOLIDCreateSweep::Modify(char * buffer)  //Script ���Ͽ� �ٷ� Class �ν��Ͻ� �̸��� ���ö� 
	{
		string token;
		char seps[] = " ,\t\n().="; //������
		char * context = NULL;			// strtok_s �Լ��� �Է� ����

		token = strtok_s(buffer, seps, &context);//rib1
		token = strtok_s(NULL, seps, &context);	//SetProfileElement

		if (token == "SetProfileElement")
		{
			token = strtok_s(NULL, seps, &context); //�ι�° ������

			sketch1 = GetPart()->GetReferenceEntityByName(token)->GetFeatureName();
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch1);
		}
		else if (token == "CenterCurveElement")
		{
			token = strtok_s(NULL, seps, &context); //�ι�° ������

			sketch2 = GetPart()->GetReferenceEntityByName(token)->GetFeatureName();
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch2);
		}

		context = NULL;
	}


}