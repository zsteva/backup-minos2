#ifndef ALSAVOLUME_H
#define ALSAVOLUME_H

class Card {
public:
    QString indexstr;
    QString name;
    QString device_name;
};

class AlsaVolume
{
public:
    AlsaVolume();
    void init();
    QVector<Card> getCardList();
    void SetAlsaPlaybackMasterVolume(long volume);
    void SetAlsaRecordMasterVolume(long volume);
};

#endif // ALSAVOLUME_H
