object LogMonitorFrame: TLogMonitorFrame
  Left = 0
  Top = 0
  Width = 320
  Height = 240
  TabOrder = 0
  TabStop = True
  object QSOTree: TVirtualStringTree
    Left = 0
    Top = 0
    Width = 320
    Height = 240
    Align = alClient
    Colors.UnfocusedColor = clMedGray
    Header.AutoSizeIndex = -1
    Header.DefaultHeight = 17
    Header.Height = 17
    Header.Options = [hoColumnResize, hoDblClickResize, hoDrag, hoVisible]
    Header.ParentFont = True
    TabOrder = 0
    TabStop = False
    TreeOptions.AutoOptions = [toAutoScrollOnExpand, toAutoTristateTracking, toAutoDeleteMovedNodes, toAutoChangeScale]
    TreeOptions.MiscOptions = [toAcceptOLEDrop, toFullRepaintOnResize, toGridExtensions, toInitOnSave, toToggleOnDblClick, toWheelPanning, toNodeHeightResize]
    TreeOptions.PaintOptions = [toHideFocusRect, toShowButtons, toShowDropmark, toShowHorzGridLines, toThemeAware, toUseBlendedImages]
    TreeOptions.SelectionOptions = [toExtendedFocus, toFullRowSelect]
    OnBeforeItemErase = QSOTreeBeforeItemErase
    OnColumnResize = QSOTreeColumnResize
    OnDblClick = QSOTreeDblClick
    OnGetText = QSOTreeGetText
    OnPaintText = QSOTreePaintText
    OnGetNodeDataSize = QSOTreeGetNodeDataSize
    Columns = <
      item
        Margin = 0
        Position = 0
        Spacing = 0
        Width = 505
      end>
  end
  object LogColumnsChangeTimer: TTimer
    OnTimer = LogColumnsChangeTimerTimer
    Left = 32
    Top = 32
  end
end
