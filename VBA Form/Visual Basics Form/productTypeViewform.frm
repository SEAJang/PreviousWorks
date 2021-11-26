VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} UserForm2 
   Caption         =   "UserForm2"
   ClientHeight    =   3375
   ClientLeft      =   120
   ClientTop       =   465
   ClientWidth     =   4260
   OleObjectBlob   =   "productTypeViewform.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "UserForm2"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False


Private Sub UserForm_Activate()

Dim ws As Worksheet
Set ws = Worksheets("mfType")
ws.Activate
ListBox1.ColumnCount = 2
ListBox1.RowSource = "mfType!A1:B50"
End Sub
