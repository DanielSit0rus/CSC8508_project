#pragma once
class ISaveable {
public:
    virtual ~ISaveable() = default;

    virtual  void RegisterSaveData() {
        ;
    }
    virtual void UnRegisterSaveData() {
        ;
    }

    virtual void SaveData() = 0;
    virtual void LoadData() = 0;
};
