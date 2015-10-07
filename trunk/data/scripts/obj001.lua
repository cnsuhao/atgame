require "scripts/base/class"
require "scripts/camera"

obj001 = { camera = nil }

function obj001:OnLoad()
    print("--->obj001:OnLoad")
    --self.camera = Camera()
    --self.camera:SetPosition(1,23,2)
    --sself.camera:GetPosition()
    
end

function obj001:OnUpdate(time)
    --self.camera:GetPosition()
end

function obj001:OnUnload()
    print("--->obj001:OnUnload")
    obj001 = nul
end

return "obj001"