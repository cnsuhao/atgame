require "scripts/base/class"

Quat = Class(function(self,x,y,z,w)   
    self.x,self.y,self.z,self.w 
    = x or 0,y or 0,z or 0,w or 0
end)

function Matrix:Set(x,y,z,w)
    self.x,self.y,self.z,self.w 
    = x or 0,y or 0,z or 0,w or 0
end

function Matrix:Get()
    return self.x,self.y,self.z,self.w
end

