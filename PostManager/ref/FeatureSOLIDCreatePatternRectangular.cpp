#include "stdafx.h"
#include "FeatureSOLIDCreatePatternRectangular.h"

namespace Post {
	FeatureSOLIDCreatePatternRectangular::FeatureSOLIDCreatePatternRectangular(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreatePatternRectangular::~FeatureSOLIDCreatePatternRectangular(void)
	{
	}

	void FeatureSOLIDCreatePatternRectangular::GetInfo(void)
	{
		TransCAD::IStdSolidOperatePatternRectangularFeaturePtr spFeature = GetTransCADFeature();
		TransCAD::IReferencesPtr spTargetFeatures = spFeature->GetTargetFeatures();

		for (int i = 1; i <= spTargetFeatures->GetCount(); ++i)
		{
			TransCAD::IReferencePtr spReference = spTargetFeatures->GetItem(i);

			cout << "	" << "Name           : " << spReference->Name << endl;
			cout << "	" << "ReferenceeName : " << spReference->ReferenceeName << endl;
			cout << "	" << "Type           : " << spReference->Type << endl;
		}

		_featurename = spTargetFeatures->GetItem(1)->GetReferenceeName();

		_colnumber = spFeature->ColumnNumber;
		_rownumber = spFeature->RowNumber;

		_colspacing = spFeature->ColumnSpacing;
		_rowspacing = spFeature->RowSpacing;

		spFeature->GetColumnDirection(&_coldir[0], &_coldir[1], &_coldir[2]);
		spFeature->GetRowDirection(&_rowdir[0], &_rowdir[1], &_rowdir[2]);

		//Rectangular Pattern�� Reference Plane ���ϱ�
		GetRectReferencePlane();

		// TransCAD Ư¡���� �̸����� CATIA Ư¡���� �̸� ���ϱ�
		TransCAD::FeatureType type;
		int _featurenum;

		g_pRefManager->GetFeatureTypeByTransCADName(_featurename, type);
		g_pRefManager->GetFeatureIDByTransCADName(_featurename, _featurenum);

		if (g_pRefManager->FeatureTypeToString(type) == "Pad")
			sprintf_s(_featurenameCATIA, "pad%d", _featurenum);
		else if (g_pRefManager->FeatureTypeToString(type) == "Pocket")
			sprintf_s(_featurenameCATIA, "pocket%d", _featurenum);
		else if (g_pRefManager->FeatureTypeToString(type) == "Rib")
			sprintf_s(_featurenameCATIA, "rib%d", _featurenum);
		else if (g_pRefManager->FeatureTypeToString(type) == "Shaft")
			sprintf_s(_featurenameCATIA, "shaft%d", _featurenum);

		cout << "  " << "ColumeNumber    : " << _colnumber << endl;
		cout << "  " << "ColumeSpacing   : " << _colspacing << endl;
		cout << "  " << "ColumeDirection : " << _coldir[0] << "," << _coldir[1] << "," << _coldir[2] << endl;
		cout << "  " << "RowNumber       : " << _rownumber << endl;
		cout << "  " << "RowSpacing      : " << _rowspacing << endl;
		cout << "  " << "RowDirection    : " << _rowdir[0] << "," << _rowdir[1] << "," << _rowdir[2] << endl;
	}

	void FeatureSOLIDCreatePatternRectangular::ToCATIA(void)
	{
		extern int ref_index;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromName(\"\")\n\n", ref_index);

		ref_index += 1;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromName(\"\")\n\n", ref_index);

		int RectpatternID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), RectpatternID))
			return;

		fprintf(GetPart()->file, "Dim rectPattern%d As RectPattern\n", RectpatternID);
		fprintf(GetPart()->file, "Set rectPattern%d = shapeFactory1.AddNewRectPattern(%s, ", RectpatternID, _featurenameCATIA);
		fprintf(GetPart()->file, "%d, %d, %f, %f, ", _colnumber, _rownumber, _colspacing*coldir, _rowspacing*rowdir);
		fprintf(GetPart()->file, "1, 1, reference%d, reference%d, True, True, 0)\n\n", ref_index - 1, ref_index);

		fprintf(GetPart()->file, "rectPattern%d.FirstRectangularPatternParameters = catInstancesandSpacing\n\n", RectpatternID);
		fprintf(GetPart()->file, "rectPattern%d.SecondRectangularPatternParameters = catInstancesandSpacing\n\n", RectpatternID);

		ref_index += 1;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = originElements1.%s\n\n", ref_index, _refPlane);
		fprintf(GetPart()->file, "rectPattern%d.SetFirstDirection reference%d\n\n", RectpatternID, ref_index);
		fprintf(GetPart()->file, "rectPattern%d.SetSecondDirection reference%d\n\n", RectpatternID, ref_index);

		fprintf(GetPart()->file, "part1.UpdateObject rectPattern%d\n\n", RectpatternID);

		ref_index += 1;
	}

	void FeatureSOLIDCreatePatternRectangular::GetRectReferencePlane()
	{
		// coldir�� rowdir�� �ľ��� ��, refplane ����
		for (int i = 0; i < 3; ++i)
		{
			if (_coldir[i] != 0) {
				coldir = _coldir[i];
				_refdir[0] = i; // column ���� ����. i = 0 �̸� X����, 1 �̸� Y����, 2 �̸� Z����.
			}

			if (_rowdir[i] != 0) {
				rowdir = _rowdir[i];
				_refdir[1] = i; // row ���� ����.
			}
		}

		// column�� row�� ������ ���� reference plane Ȯ��. 0,1,2 ������� X,Y,Z�� �ǹ���
		if ((_refdir[0] == 0 && _refdir[1] == 1) || (_refdir[0] == 1 && _refdir[1] == 0)) //XY Plane
			sprintf_s(_refPlane, "%s", "PlaneXY");
		else if ((_refdir[0] == 1 && _refdir[1] == 2) || (_refdir[0] == 2 && _refdir[1] == 1)) //YZ Plane
			sprintf_s(_refPlane, "%s", "PlaneYZ");
		else if ((_refdir[0] == 0 && _refdir[1] == 2) || (_refdir[0] == 2 && _refdir[1] == 0)) //ZX Plane
			sprintf_s(_refPlane, "%s", "PlaneZX");

		// XZ Plane�� ���, X�� Z�� ������ ���� �ٲپ� ZX Plane���� �ٲ�
		// �ֳ��ϸ� TransCAD ������ XZ, CATIA������ ZX�� �ν��ϱ� ����.
		if (_refdir[0] == 0 && _refdir[1] == 2)
		{
			double temp;

			temp = (double)_rownumber;
			_rownumber = _colnumber;
			_colnumber = (int)temp;

			temp = _rowspacing;
			_rowspacing = _colspacing;
			_colspacing = temp;

			temp = rowdir;
			rowdir = coldir;
			coldir = temp;
		}
	}

}