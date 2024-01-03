#pragma once

struct IGuiComponent {
    virtual ~IGuiComponent() = default;

    virtual bool Initialize() = 0;
    virtual void Render() = 0;
    virtual void Update() = 0;
};
