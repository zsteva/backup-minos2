object KeyControlForm: TKeyControlForm
  Left = 401
  Top = 238
  Caption = 'G0GJV Contest Keyer Control'
  ClientHeight = 336
  ClientWidth = 603
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnKeyUp = FormKeyUp
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 256
    Height = 336
    ActivePage = VoiceTab
    Align = alClient
    TabOrder = 0
    object VoiceTab: TTabSheet
      Caption = 'Voice'
      object SPanel1: TSPanel
        Left = 0
        Top = 0
        Width = 248
        Height = 308
        Align = alClient
        BevelOuter = bvNone
        BevelWidth = 0
        TabOrder = 0
        object Label1: TLabel
          Left = 16
          Top = 50
          Width = 18
          Height = 13
          Caption = 'Key'
        end
        object recind: TLabel
          Left = 144
          Top = 48
          Width = 29
          Height = 13
          Caption = 'recind'
        end
        object Label2: TLabel
          Left = -1
          Top = 18
          Width = 224
          Height = 13
          Caption = 'Shiftt/Fx to play x, Shift/Ctrl/Fx to start record x)'
        end
        object Label3: TLabel
          Left = 1
          Top = 120
          Width = 52
          Height = 13
          Caption = 'Input Lines'
        end
        object Label4: TLabel
          Left = 1
          Top = 144
          Width = 60
          Height = 13
          Caption = 'Output Lines'
        end
        object Label17: TLabel
          Left = 111
          Top = 169
          Width = 58
          Height = 13
          Caption = 'Delay (secs)'
        end
        object RecordButton: TButton
          Left = 2
          Top = 80
          Width = 49
          Height = 25
          Caption = 'Record'
          TabOrder = 1
          OnClick = RecordButtonClick
        end
        object StopButton: TButton
          Left = 178
          Top = 80
          Width = 49
          Height = 25
          Cancel = True
          Caption = 'Stop'
          TabOrder = 4
          OnClick = StopButtonClick
        end
        object PlayButton: TButton
          Left = 60
          Top = 80
          Width = 49
          Height = 25
          Caption = 'Play'
          TabOrder = 2
          OnClick = PlayButtonClick
        end
        object KeyCombo: TComboBox
          Left = 58
          Top = 46
          Width = 49
          Height = 21
          Style = csDropDownList
          ItemHeight = 13
          ItemIndex = 0
          TabOrder = 0
          Text = '1'
          Items.Strings = (
            '1'
            '2'
            '3'
            '4'
            '5'
            '6'
            '7'
            '8'
            '9')
        end
        object PTTOutCheckBox: TCheckBox
          Left = 77
          Top = 142
          Width = 41
          Height = 17
          Caption = 'PTT'
          Enabled = False
          TabOrder = 8
        end
        object KeyCheckBox: TCheckBox
          Left = 124
          Top = 143
          Width = 44
          Height = 17
          Caption = 'Key'
          Enabled = False
          TabOrder = 9
        end
        object PTTReflectCheckBox: TCheckBox
          Left = 77
          Top = 119
          Width = 41
          Height = 17
          Caption = 'PTT'
          Enabled = False
          TabOrder = 5
        end
        object L1ReflectCheckBox: TCheckBox
          Left = 124
          Top = 119
          Width = 36
          Height = 17
          Caption = 'L1'
          Enabled = False
          TabOrder = 6
        end
        object L2ReflectCheckBox: TCheckBox
          Left = 166
          Top = 119
          Width = 34
          Height = 17
          Caption = 'L2'
          Enabled = False
          TabOrder = 7
        end
        object PipCheckBox: TCheckBox
          Left = 16
          Top = 257
          Width = 89
          Height = 17
          Caption = 'Use Pip Tone'
          TabOrder = 10
          OnClick = PipCheckBoxClick
        end
        object Tone1Button: TButton
          Left = 24
          Top = 219
          Width = 75
          Height = 25
          Caption = 'Tune'
          TabOrder = 11
          OnClick = Tone1ButtonClick
        end
        object Tone2Button: TButton
          Left = 136
          Top = 219
          Width = 75
          Height = 25
          Caption = 'Two Tone'
          TabOrder = 12
          OnClick = Tone2ButtonClick
        end
        object AboutButton: TButton
          Left = 136
          Top = 256
          Width = 75
          Height = 25
          Caption = 'About...'
          TabOrder = 13
          OnClick = AboutButtonClick
        end
        object ShowButton: TButton
          Left = 121
          Top = 80
          Width = 49
          Height = 25
          Cancel = True
          Caption = 'Show'
          TabOrder = 3
          OnClick = ShowButtonClick
        end
        object AutoRepeatCheckBox: TCheckBox
          Left = 0
          Top = 168
          Width = 97
          Height = 17
          Caption = 'Auto Repeat'
          TabOrder = 14
          OnClick = AutoRepeatCheckBoxClick
        end
        object DelayEdit: TEdit
          Left = 174
          Top = 166
          Width = 49
          Height = 21
          TabOrder = 15
          Text = '0'
          OnChange = DelayEditChange
        end
        object DelayUpDown: TUpDown
          Left = 223
          Top = 166
          Width = 16
          Height = 21
          Associate = DelayEdit
          TabOrder = 16
        end
      end
    end
    object CWTab: TTabSheet
      Caption = 'CW'
      ImageIndex = 1
      object Label13: TLabel
        Left = 8
        Top = 8
        Width = 62
        Height = 13
        Caption = 'Speed (wpm)'
      end
      object Label14: TLabel
        Left = 136
        Top = 8
        Width = 44
        Height = 13
        Caption = 'Tone(Hz)'
      end
      object Memo1: TMemo
        Left = 8
        Top = 64
        Width = 233
        Height = 137
        ScrollBars = ssVertical
        TabOrder = 0
      end
      object SendButton: TButton
        Left = 8
        Top = 216
        Width = 75
        Height = 25
        Caption = 'Send'
        TabOrder = 1
        OnClick = SendButtonClick
      end
      object CWStopButton: TButton
        Left = 80
        Top = 254
        Width = 75
        Height = 25
        Caption = 'Stop'
        TabOrder = 2
        OnClick = StopButtonClick
      end
      object ClearButton: TButton
        Left = 152
        Top = 216
        Width = 75
        Height = 25
        Caption = 'Clear'
        TabOrder = 3
        OnClick = ClearButtonClick
      end
      object SpeedControl: TEdit
        Left = 80
        Top = 6
        Width = 38
        Height = 21
        TabOrder = 4
        Text = '12'
        OnChange = SpeedControlChange
        OnKeyPress = ToneEditKeyPress
      end
      object ToneEdit: TEdit
        Left = 192
        Top = 8
        Width = 57
        Height = 21
        TabOrder = 5
        Text = '1000'
        OnChange = ToneEditChange
        OnKeyPress = ToneEditKeyPress
      end
      object CWFileEdit: TEdit
        Left = 8
        Top = 32
        Width = 105
        Height = 21
        TabOrder = 6
      end
      object CWBrowse: TButton
        Left = 124
        Top = 31
        Width = 32
        Height = 25
        Caption = '...'
        TabOrder = 7
        OnClick = CWBrowseClick
      end
      object SendFileButton: TButton
        Left = 165
        Top = 32
        Width = 75
        Height = 25
        Caption = 'Send File'
        TabOrder = 8
        OnClick = SendFileButtonClick
      end
      object SpeedUpDown: TUpDown
        Left = 118
        Top = 6
        Width = 16
        Height = 21
        Associate = SpeedControl
        Min = 5
        Max = 30
        Position = 12
        TabOrder = 9
      end
    end
    object Mixer: TTabSheet
      Caption = 'Mixer'
      ImageIndex = 2
      object Label18: TLabel
        Left = 16
        Top = 41
        Width = 52
        Height = 13
        Caption = 'Input Mixer'
      end
      object Label19: TLabel
        Left = 16
        Top = 140
        Width = 60
        Height = 13
        Caption = 'Output Mixer'
      end
      object Label20: TLabel
        Left = 16
        Top = 84
        Width = 34
        Height = 13
        Caption = 'Source'
      end
      object Label21: TLabel
        Left = 14
        Top = 186
        Width = 53
        Height = 13
        Caption = 'Destination'
      end
      object Label22: TLabel
        Left = 16
        Top = 237
        Width = 209
        Height = 26
        Caption = 
          'Changing mixers or lines will cause the keyer to close and be re' +
          'started.'
        WordWrap = True
      end
      object InputMixerComboBox: TComboBox
        Left = 14
        Top = 59
        Width = 213
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnChange = InputMixerComboBoxChange
      end
      object OutputMixerComboBox: TComboBox
        Left = 14
        Top = 159
        Width = 213
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
        OnChange = OutputMixerComboBoxChange
      end
      object SourceComboBox: TComboBox
        Left = 14
        Top = 103
        Width = 213
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 2
        OnChange = SourceLineComboBoxChange
      end
      object DestinationComboBox: TComboBox
        Left = 14
        Top = 202
        Width = 213
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 3
        OnChange = DestinationComboBoxChange
      end
    end
  end
  object SPanel2: TSPanel
    Left = 256
    Top = 0
    Width = 347
    Height = 336
    Align = alRight
    BevelInner = bvRaised
    TabOrder = 1
    object SPanel3: TSPanel
      Left = 2
      Top = 2
      Width = 343
      Height = 332
      Align = alClient
      BevelOuter = bvSpace
      BevelWidth = 0
      TabOrder = 0
      object Label5: TLabel
        Left = 59
        Top = 9
        Width = 35
        Height = 13
        Caption = 'Mic Vol'
      end
      object Label6: TLabel
        Left = 116
        Top = 9
        Width = 49
        Height = 13
        Caption = 'Rec Level'
      end
      object Label8: TLabel
        Left = 287
        Top = 9
        Width = 50
        Height = 13
        Caption = 'Master Vol'
      end
      object Label9: TLabel
        Left = 63
        Top = 259
        Width = 32
        Height = 13
        Caption = 'Label9'
      end
      object Label10: TLabel
        Left = 120
        Top = 259
        Width = 38
        Height = 13
        Caption = 'Label10'
      end
      object Label12: TLabel
        Left = 291
        Top = 259
        Width = 38
        Height = 13
        Caption = 'Label12'
      end
      object Label7: TLabel
        Left = 173
        Top = 9
        Width = 53
        Height = 13
        Caption = 'Mic out Vol'
      end
      object Label11: TLabel
        Left = 177
        Top = 259
        Width = 38
        Height = 13
        Caption = 'Label11'
      end
      object Label15: TLabel
        Left = 230
        Top = 9
        Width = 49
        Height = 13
        Caption = 'Wave Out'
      end
      object Label16: TLabel
        Left = 234
        Top = 259
        Width = 38
        Height = 13
        Caption = 'Label16'
      end
      object MUXSelection: TLabel
        Left = 64
        Top = 304
        Width = 249
        Height = 13
        AutoSize = False
        Caption = 'MUXSelection'
        Visible = False
      end
      object CommonFBVUMeter: TFBVUMeter
        Left = 15
        Top = 8
        Width = 32
        Height = 289
        levelMin = -50
        stereo = False
      end
      object MicRecTrackBar: TxTrackBar
        Left = 63
        Top = 30
        Width = 33
        Height = 227
        LineSize = 100
        Max = 65535
        Orientation = trVertical
        PageSize = 100
        Frequency = 3320
        TabOrder = 1
        OnChange = MicRecTrackBarChange
      end
      object RecordTrackBar: TxTrackBar
        Left = 120
        Top = 30
        Width = 33
        Height = 227
        LineSize = 100
        Max = 65535
        Orientation = trVertical
        PageSize = 100
        Frequency = 3320
        TabOrder = 2
        OnChange = RecordTrackBarChange
      end
      object OutputTrackBar: TxTrackBar
        Left = 291
        Top = 30
        Width = 33
        Height = 227
        LineSize = 100
        Max = 65535
        Orientation = trVertical
        PageSize = 100
        Frequency = 3320
        TabOrder = 7
        OnChange = OutputTrackBarChange
      end
      object MicOutTrackBar: TxTrackBar
        Left = 177
        Top = 30
        Width = 33
        Height = 227
        LineSize = 100
        Max = 65535
        Orientation = trVertical
        PageSize = 100
        Frequency = 3320
        TabOrder = 3
        OnChange = MicOutTrackBarChange
      end
      object MicOutMute: TCheckBox
        Left = 176
        Top = 280
        Width = 49
        Height = 17
        Caption = 'Mute'
        Enabled = False
        TabOrder = 4
      end
      object MasterMute: TCheckBox
        Left = 288
        Top = 280
        Width = 49
        Height = 17
        Caption = 'Mute'
        Enabled = False
        TabOrder = 8
      end
      object WaveOutTrackBar: TxTrackBar
        Left = 234
        Top = 30
        Width = 33
        Height = 227
        LineSize = 100
        Max = 65535
        Orientation = trVertical
        PageSize = 100
        Frequency = 3320
        TabOrder = 5
        OnChange = WaveOutTrackBarChange
      end
      object WaveOutMute: TCheckBox
        Left = 232
        Top = 281
        Width = 49
        Height = 17
        Caption = 'Mute'
        Enabled = False
        TabOrder = 6
      end
    end
  end
  object LineTimer: TTimer
    Interval = 100
    OnTimer = LineTimerTimer
    Left = 240
    Top = 64
  end
  object TreeImageList: TImageList
    Left = 240
    Top = 40
    Bitmap = {
      494C010106000A001C0010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000002000000001002000000000000020
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000FFFFFF000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000FFFFFF000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000000000FF
      FF00C6C6C60000FFFF00C6C6C60000FFFF00C6C6C60000FFFF0000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF000000
      000000FFFF00C6C6C60000FFFF00C6C6C60000FFFF00C6C6C60000FFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF0000FF
      FF000000000000FFFF00C6C6C60000FFFF00C6C6C60000FFFF00C6C6C60000FF
      FF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000FF
      FF0000FFFF000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF00FFFF
      FF0000FFFF000000000000FFFF000000000000FFFF000000000000FFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00000000000000000000000000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF00000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF0000FFFF00FFFFFF0000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF000000000000000000FFFFFF00FFFFFF0000000000FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FF
      FF00FFFFFF0000FFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00848484000000
      000000000000000000000000000084848400FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00848484000000
      000000000000000000000000000084848400FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00424D3E000000000000003E000000
      2800000040000000200000000100010000000000000100000000000000000000
      000000000000000000000000FFFFFF000000FFFF000000000000FFFF00000000
      0000FFFF000000000000C447000000000000C447000000000000D92700000000
      0000E7DF000000000000C7C7000000000000DFE7000000000000C7C700000000
      0000E7DF000000000000D927000000000000D927000000000000C44700000000
      0000FFFF000000000000FFFF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000}
  end
  object ConnectTimer: TTimer
    OnTimer = ConnectTimerTimer
    Left = 240
    Top = 96
  end
  object LogTimer: TTimer
    Interval = 100
    OnTimer = LogTimerTimer
    Left = 240
    Top = 128
  end
  object CWOpenDialog: TOpenDialog
    Filter = 'All files|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 208
    Top = 8
  end
  object StartupTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = StartupTimerTimer
    Left = 240
    Top = 160
  end
  object CaptionTimer: TTimer
    Enabled = False
    Interval = 200
    OnTimer = CaptionTimerTimer
    Left = 240
    Top = 192
  end
end
