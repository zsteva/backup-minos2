object QSOEditDlg: TQSOEditDlg
  Left = 0
  Top = 0
  Caption = 'QSOEditDlg'
  ClientHeight = 446
  ClientWidth = 631
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnKeyUp = FormKeyUp
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object QSOHistoryTree: TVirtualStringTree
    Left = 0
    Top = 0
    Width = 631
    Height = 256
    Align = alClient
    Header.AutoSizeIndex = 0
    Header.DefaultHeight = 17
    Header.Font.Charset = DEFAULT_CHARSET
    Header.Font.Color = clWindowText
    Header.Font.Height = -11
    Header.Font.Name = 'Verdana'
    Header.Font.Style = []
    Header.MainColumn = -1
    Header.Options = [hoColumnResize, hoDrag]
    TabOrder = 0
    TabStop = False
    OnBeforeItemErase = QSOHistoryTreeBeforeItemErase
    OnGetText = QSOHistoryTreeGetText
    OnGetNodeDataSize = QSOHistoryTreeGetNodeDataSize
    OnInitChildren = QSOHistoryTreeInitChildren
    OnInitNode = QSOHistoryTreeInitNode
    Columns = <>
  end
  inline GJVQSOEditFrame: TGJVQSOEditFrame
    Left = 0
    Top = 256
    Width = 631
    Height = 190
    Align = alBottom
    TabOrder = 1
    ExplicitTop = 256
    ExplicitWidth = 631
    ExplicitHeight = 190
    inherited G0GJVPanel: TPanel
      Width = 631
      Height = 190
      ExplicitWidth = 631
      ExplicitHeight = 190
      inherited GJVCancelButton: TButton [5]
        OnClick = GJVQSOEditFrame1GJVCancelButtonClick
      end
      inherited GJVForceButton: TButton [6]
        OnClick = GJVQSOEditFrame1GJVForceButtonClick
      end
      inherited NonScoreCheckBox: TCheckBox [7]
      end
      inherited DeletedCheckBox: TCheckBox [8]
      end
      inherited MainOpComboBox: TComboBox [9]
      end
      inherited SerRXEdit: TLabeledEdit [10]
      end
      inherited LocEdit: TLabeledEdit [11]
      end
      inherited QTHEdit: TLabeledEdit [12]
      end
      inherited CommentsEdit: TLabeledEdit [13]
      end
      inherited ModeComboBoxGJV: TComboBox [14]
      end
      inherited GJVOKButton: TButton [18]
        OnClick = GJVQSOEditFrame1GJVOKButtonClick
      end
      inherited CatchupButton: TButton [19]
      end
      inherited SecondOpComboBox: TComboBox [20]
      end
      inherited RSTRXEdit: TLabeledEdit [21]
      end
      inherited SerTXEdit: TLabeledEdit [22]
      end
      inherited RSTTXEdit: TLabeledEdit [23]
      end
    end
  end
  object InitialiseTimer: TTimer
    Enabled = False
    Interval = 10
    OnTimer = InitialiseTimerTimer
    Left = 40
    Top = 72
  end
end
