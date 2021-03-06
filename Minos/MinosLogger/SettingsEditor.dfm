object SettingsEditDlg: TSettingsEditDlg
  Left = 355
  Top = 164
  Caption = 'Settings Editor'
  ClientHeight = 309
  ClientWidth = 736
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Verdana'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 268
    Width = 736
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitTop = 475
    DesignSize = (
      736
      41)
    object OKButton: TButton
      Left = 616
      Top = 10
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '&OK'
      Default = True
      TabOrder = 0
      OnClick = OKButtonClick
    end
    object CopyButton: TButton
      Left = 157
      Top = 9
      Width = 89
      Height = 25
      Caption = 'Copy Setting'
      TabOrder = 3
      OnClick = CopyButtonClick
    end
    object DeleteButton: TButton
      Left = 291
      Top = 10
      Width = 99
      Height = 25
      Caption = 'Delete Setting'
      TabOrder = 4
      OnClick = DeleteButtonClick
    end
    object CancelButton: TButton
      Left = 504
      Top = 10
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Cancel = True
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
      OnClick = CancelButtonClick
    end
    object NewSectionButton: TButton
      Left = 24
      Top = 10
      Width = 75
      Height = 25
      Caption = 'New Setting'
      TabOrder = 2
      OnClick = NewSectionButtonClick
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 736
    Height = 268
    Align = alClient
    TabOrder = 1
    ExplicitHeight = 475
    object SectionsList: TListBox
      Left = 1
      Top = 1
      Width = 184
      Height = 266
      Align = alLeft
      ItemHeight = 13
      TabOrder = 0
      OnClick = SectionsListClick
      ExplicitHeight = 473
    end
    object OptionsScrollBox: TScrollBox
      Left = 185
      Top = 1
      Width = 550
      Height = 266
      Align = alClient
      TabOrder = 1
      ExplicitHeight = 473
    end
  end
end
