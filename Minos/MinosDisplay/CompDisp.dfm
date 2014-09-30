object CompFrame: TCompFrame
  Left = 0
  Top = 0
  Width = 320
  Height = 240
  TabOrder = 0
  object CompGrid: TVirtualStringTree
    Left = 0
    Top = 0
    Width = 320
    Height = 240
    Align = alClient
    Colors.UnfocusedColor = clMedGray
    Header.AutoSizeIndex = 0
    Header.DefaultHeight = 17
    Header.Height = 17
    Header.MainColumn = -1
    Header.ParentFont = True
    TabOrder = 0
    TreeOptions.AutoOptions = [toAutoScrollOnExpand, toAutoTristateTracking, toAutoDeleteMovedNodes, toAutoChangeScale]
    TreeOptions.MiscOptions = [toAcceptOLEDrop, toFullRepaintOnResize, toGridExtensions, toInitOnSave, toToggleOnDblClick, toWheelPanning, toNodeHeightResize]
    TreeOptions.PaintOptions = [toShowButtons, toShowDropmark, toShowHorzGridLines, toShowVertGridLines, toThemeAware, toUseBlendedImages, toFullVertGridLines]
    OnGetText = CompGridGetText
    OnGetNodeDataSize = CompGridGetNodeDataSize
    OnInitChildren = CompGridInitChildren
    OnInitNode = CompGridInitNode
    Columns = <>
  end
  object RecheckTimer: TTimer
    Interval = 10000
    OnTimer = RecheckTimerTimer
    Left = 8
    Top = 8
  end
end
