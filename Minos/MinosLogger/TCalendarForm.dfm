object CalendarForm: TCalendarForm
  Left = 0
  Top = 0
  Caption = 'Select Contest from VHF Calendar'
  ClientHeight = 334
  ClientWidth = 748
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
  object CalendarGrid: TStringGrid
    Left = 0
    Top = 0
    Width = 748
    Height = 293
    Align = alClient
    FixedCols = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goRowSelect, goThumbTracking]
    TabOrder = 0
    OnDblClick = CalendarGridDblClick
  end
  object Panel1: TPanel
    Left = 0
    Top = 293
    Width = 748
    Height = 41
    Align = alBottom
    TabOrder = 1
    ExplicitLeft = -176
    ExplicitTop = 0
    DesignSize = (
      748
      41)
    object CalendarVersionLabel: TLabel
      Left = 292
      Top = 14
      Width = 124
      Height = 13
      Caption = 'CalendarVersionLabel'
    end
    object CloseButton: TButton
      Left = 651
      Top = 6
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Cancel = True
      Caption = 'Close'
      ModalResult = 2
      TabOrder = 0
      OnClick = CloseButtonClick
    end
    object SelectButton: TButton
      Left = 14
      Top = 6
      Width = 101
      Height = 25
      Caption = 'Select Contest'
      Default = True
      ModalResult = 1
      TabOrder = 1
      OnClick = SelectButtonClick
    end
    object GetCalendarButton: TButton
      Left = 121
      Top = 6
      Width = 160
      Height = 25
      Caption = 'Download latest Calendar'
      TabOrder = 2
      OnClick = GetCalendarButtonClick
    end
    object YearEdit: TEdit
      Left = 531
      Top = 10
      Width = 65
      Height = 21
      ReadOnly = True
      TabOrder = 3
      Text = 'YearEdit'
    end
    object YearDownButton: TButton
      Left = 496
      Top = 8
      Width = 26
      Height = 25
      Caption = '<<'
      TabOrder = 4
      OnClick = YearDownButtonClick
    end
    object YearUpButton: TButton
      Left = 605
      Top = 8
      Width = 31
      Height = 25
      Caption = '>>'
      TabOrder = 5
      OnClick = YearUpButtonClick
    end
  end
  object IdHTTP1: TIdHTTP
    AllowCookies = True
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.Accept = 'text/html;*/*'
    Request.BasicAuthentication = False
    Request.UserAgent = 'Mozilla/4.0 (compatible;Minos)'
    HTTPOptions = [hoForceEncodeParams]
    Left = 40
    Top = 464
  end
end
