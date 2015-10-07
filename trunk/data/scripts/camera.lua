require "scripts/base/class"
require "scripts/base/vector3.lua"
require "scripts/base/quad.lua"

Camera = Class(function(self)
    self.inst = CreateCamera()
end)

function Camera:SetPosition(x, y, z)
    CameraSetPosition(self.inst, x, y, z)
end

function Camera:GetPosition()
    return Vector3(CameraGetPosition(self.inst))
end

function Camera:SetLookAt(Pos, focus, up)
    CameraSetLookAt(self.inst, pos:Get(), focus:Get(), up:Get())
end

function Camera:SetForward(forward)
    CameraSetForward(self.inst,forward:Get())
end

function Camera:GetForward()
    return Vector3(self.inst,CameraGetForward())
end

function Camera:GetRight()
    return Vector3(self.inst,CameraGetRight())
end

function Camera:GetForward()
    return Vector3(self.inst,CameraGetForward())
end

function Camera:SetUp(up)
    CameraSetUp(self.inst,up:Get())
end

function Camera:GetUp()
    return Vector3(CameraGetUp(self.inst))
end

function Camera:SetYaw(Yaw)
    CameraSetYaw(self.inst,yaw)
end

function Camera:SetPitch(Pitch)
    CameraSetPitch(self.inst,Pitch)
end

function Camera:SetRoll(float Roll)
    CameraSetRoll(self.inst,Rool)
end

function Camera:SetRotation(rotation)
    CameraSetRotation(self.inst, rotation:Get())
end

function Camera:GetRotation()
    return Quat(CameraGetRotation(self.inst))
end

function Camera:SetProjection(Fov_y, Aspect, ZNear, ZFar)
    CameraSetProjection(self.inst, Fov_y, Aspect, ZNear, ZFar)
end

function Camera:SetFov(Fov_y)
    CameraSetFov(self.inst, Fov_y)
end

function Camera:SetAspect(Aspect)
    CameraSetAspect(self.inst, Aspect)
end

function Camera:SetClipNear(float ZNear)
    CameraSetClipNear(self.inst, ZNear)
end

function Camera:SetClipFar(float ZFar)
    CameraSetClipFar(self.inst, ZFar)
end

function Camera:Update()
    return CameraUpdate(self.inst)
end

function Camera:GetView()
    return Vector3(CameraGetView(self.inst))
end

function Camera:GetProj()
    return Vector3(CameraGetProj(self.inst))
end

function Camera:DestoryCamera()
    DestoryCamera(self.inst);
    self.inst = nil
end
