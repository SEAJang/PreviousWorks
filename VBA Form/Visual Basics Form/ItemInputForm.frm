VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} UserForm1 
   Caption         =   "UserForm1"
   ClientHeight    =   12000
   ClientLeft      =   120
   ClientTop       =   465
   ClientWidth     =   20670
   OleObjectBlob   =   "ItemInputForm.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "UserForm1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub Add_Click()

Dim wks As Worksheet
Dim addNew As Range
Set wks = Sheet2

If productCode_txt.Text = "" Then
    MsgBox ("Please enter the New Scan Code")
Else
    Set addNew = Worksheets("NewData").Range("A65356").End(xlUp).Offset(1, 0)
    
    addNew.Offset(0, 0).Value = productCode_txt.Text
    addNew.Offset(0, 1).Value = SKU_txt.Text
    addNew.Offset(0, 2).Value = prodName_txt.Text
    addNew.Offset(0, 3).Value = prodNameK_txt.Text
    addNew.Offset(0, 4).Value = IUPrice_txt.Text
    addNew.Offset(0, 5).Value = OUPrice_txt.Text
    addNew.Offset(0, 6).Value = unit_txt.Text
    addNew.Offset(0, 7).Value = prodtype_txt.Text
    addNew.Offset(0, 8).Value = carryalong_txt.Text
    addNew.Offset(0, 9).Value = TaxCode_txt.Text
    addNew.Offset(0, 10).Value = supplier_txt.Text
    addNew.Offset(0, 11).Value = qtyABreak_txt.Text
    addNew.Offset(0, 12).Value = APrice_txt.Text
    addNew.Offset(0, 13).Value = serialNo_txt.Text
    addNew.Offset(0, 14).Value = promoOnOff_txt.Text
    addNew.Offset(0, 15).Value = promoPrice_txt.Text
    
End If
DataDisplay.ColumnCount = 16
DataDisplay.RowSource = "NewData!A1:P65356"

End Sub

Private Sub CommandButton1_Click()

MsgBox ("Product Name, English Only")
End Sub

Private Sub CommandButton10_Click()
MsgBox ("Sale ABreak Quantity")
End Sub

Private Sub CommandButton11_Click()
MsgBox ("Sale ABreak Price")
End Sub

Private Sub CommandButton13_Click()
MsgBox ("Mix and Match Code")
End Sub

Private Sub CommandButton14_Click()
End Sub

Private Sub CommandButton2_Click()
MsgBox ("Sale Price")
End Sub

Private Sub CommandButton3_Click()
MsgBox ("Units: EA, BX, PK, LB")
End Sub

Private Sub CommandButton4_Click()
MsgBox ("Deposit/Recycle Fee Items")
End Sub

Private Sub CommandButton5_Click()

MsgBox ("Purchase Price")
End Sub

Private Sub CommandButton6_Click()
MsgBox ("Product Name, Korean")
End Sub

Private Sub CommandButton7_Click()
Dim f As New UserForm2
f.Show
End Sub

Private Sub Delete_Click()

Dim i As Integer
Dim ws As Worksheet
Set ws = Worksheets("NewData")
ws.Activate
iDelete = MsgBox("Do you want to delete this data", vbQuestion + vbYesNo)
If iDelete = vbYes Then

For i = 0 To Worksheets("NewData").Range("A65356").End(xlUp).Row - 1
    If DataDisplay.Selected(i) Then
        Rows(i + 1).Select
        Selection.Delete
    End If
Next i
End If

DataDisplay.ColumnCount = 16
DataDisplay.RowSource = "NewData!A1:P65356"

End Sub

Private Sub exitbtn_Click()

Dim iExit As VbMsgBoxResult

iExit = MsgBox("Confirm if you would like to exit", vbQuestion + vbYesNo, "Data Entry Form")

If iExit = vbYes Then
Unload Me
End If

End Sub

Private Sub UserForm_Activate()

Dim od As Worksheet
Set od = Worksheets("mfProd")
od.Activate
ListBox1.ColumnCount = 30
ListBox1.RowSource = "mfProd!A1:AC65356"
End Sub


Private Sub ProductCode_Help_Click()
MsgBox ("Old Barcode")
End Sub

Private Sub RefreshBtn_Click()

Dim ws As Worksheet
Set ws = Worksheets("NewData")
ws.Activate
DataDisplay.ColumnCount = 16
DataDisplay.RowSource = "NewData!A1:P65356"


Dim od As Worksheet
Set od = Worksheets("mfProd")
od.Activate
ListBox1.ColumnCount = 30
ListBox1.RowSource = "mfProd!A1:AC65356"

End Sub

Private Sub Reset_Click()

Dim iControl As Control

For Each iControl In Me.Controls
If iControl.Name Like "*txt" Then iControl = vbNullString
Next


End Sub

Private Sub Search_Click()

Dim ws As Worksheet
Set ws = Worksheets("mfProd")
ws.Activate

Dim iSearch As Long, i As Long

iSearch = Worksheets("mfProd").Range("A2").CurrentRegion.Rows.Count

For i = 2 To iSearch

    If Trim(Sheet1.Cells(i, 1)) <> Trim(SKU_txt.Text) And i = iSearch Then
    
    Dim iSearching As VbMsgBoxResult
        iSearching = MsgBox("No Items Found. Would you like to add?", vbQuestion + vbYesNo)
        If iSearching = vbNo Then
            SKU_txt.Text = ""
            SKU_txt.SetFocus
        End If
    End If
    
    If Trim(Sheet1.Cells(i, 1)) = Trim(SKU_txt.Text) Then
    
        prodName_txt.Text = Sheet1.Cells(i, 2)
        prodNameK_txt.Text = Sheet1.Cells(i, 3)
        IUPrice_txt.Text = Sheet1.Cells(i, 6)
        OUPrice_txt.Text = Sheet1.Cells(i, 7)
        unit_txt.Text = Sheet1.Cells(i, 12)
        prodtype_txt.Text = Sheet1.Cells(i, 4)
        TaxCode_txt.Text = Sheet1.Cells(i, 11)
        supplier_txt.Text = Sheet1.Cells(i, 5)
        promoOnOff_txt.Text = "0"
        carryalong_txt.Text = "0"

Exit For
End If
Next i


End Sub
