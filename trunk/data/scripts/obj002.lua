obj001 = { dist = 1000; camera = nil; }

function obj001:OnLoad()
    print("--->obj001:OnLoad")
end

function obj001:OnUpdate(time)
    self.camera = GetDefaultCamera();
    if self.camera ~= nil then
        self.dist = self.dist - 5;
        if self.dist <= 0 then
            self.dist = 1000
        end
        CameraSetPosition(self.camera, 0, 100, self.dist);
    end 
end

function obj001:OnUnload()
    print("--->obj001:OnUnload")
    obj001 = nul
end

return "obj001"