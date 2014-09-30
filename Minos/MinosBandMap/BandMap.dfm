object BandMapForm: TBandMapForm
  Left = 787
  Top = 121
  Caption = 'Minos Band Map'
  ClientHeight = 604
  ClientWidth = 467
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Verdana'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 512
    Width = 59
    Height = 13
    Caption = 'Frequency'
  end
  object FreqLabel: TLabel
    Left = 96
    Top = 512
    Width = 55
    Height = 13
    Caption = 'FreqLabel'
  end
  object Label3: TLabel
    Left = 8
    Top = 568
    Width = 54
    Height = 13
    Caption = 'Log entry'
  end
  object LogLabel: TLabel
    Left = 96
    Top = 568
    Width = 50
    Height = 13
    Caption = 'LogLabel'
  end
  object BMTree: TVirtualStringTree
    Left = 8
    Top = 8
    Width = 449
    Height = 473
    Colors.UnfocusedColor = clMedGray
    Header.AutoSizeIndex = -1
    Header.DefaultHeight = 17
    Header.Font.Charset = DEFAULT_CHARSET
    Header.Font.Color = clWindowText
    Header.Font.Height = -11
    Header.Font.Name = 'MS Sans Serif'
    Header.Font.Style = []
    Header.Height = 17
    Header.Options = [hoColumnResize, hoDrag, hoVisible]
    TabOrder = 0
    TreeOptions.AutoOptions = [toAutoScrollOnExpand, toAutoTristateTracking, toAutoDeleteMovedNodes, toAutoChangeScale, toDisableAutoscrollOnEdit]
    TreeOptions.MiscOptions = [toAcceptOLEDrop, toFullRepaintOnResize, toGridExtensions, toInitOnSave, toToggleOnDblClick, toWheelPanning, toNodeHeightResize]
    TreeOptions.PaintOptions = [toHideFocusRect, toShowButtons, toShowDropmark, toShowHorzGridLines, toShowVertGridLines, toThemeAware, toUseBlendedImages]
    TreeOptions.SelectionOptions = [toExtendedFocus, toFullRowSelect, toCenterScrollIntoView]
    OnGetText = BMTreeGetText
    OnGetNodeDataSize = BMTreeGetNodeDataSize
    Columns = <
      item
        Position = 0
      end>
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 16
    Top = 40
  end
  object LogTimer: TTimer
    Interval = 100
    OnTimer = LogTimerTimer
    Left = 96
    Top = 40
  end
  object StartupTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = StartupTimerTimer
    Left = 24
    Top = 104
  end
end
