require "scripts/base/class"

Matrix = Class(function(self,
                    m1,m2,m3,m4,
                    m5,m6,m7,m8,
                    m9,m10,m11,m12,
                    m13,m14,m15,m16)
                    
    self.m1,self.m2,self.m3,self.m4,
    self.m5,self.m6,self.m7,self.m8,
    self.m9,self.m10,self.m11,self.m12,
    self.m13,self.m14,self.m15,self.m16 
    = m1 or 0,m2 or 0,m3 or 0,m4 or 0,
      m5 or 0,m6 or 0,m7 or 0,m8 or 0,
      m9 or 0,m10 or 0,m11 or 0,m12 or 0,
      m13 or 0,m14 or 0,m15 or 0,m16 or 0
end)

function Matrix:Set(m1,m2,m3,m4,
                    m5,m6,m7,m8,
                    m9,m10,m11,m12,
                    m13,m14,m15,m16)
                    
    self.m1,self.m2,self.m3,self.m4,
    self.m5,self.m6,self.m7,self.m8,
    self.m9,self.m10,self.m11,self.m12,
    self.m13,self.m14,self.m15,self.m16 
    = m1 or 0,m2 or 0,m3 or 0,m4 or 0,
      m5 or 0,m6 or 0,m7 or 0,m8 or 0,
      m9 or 0,m10 or 0,m11 or 0,m12 or 0,
      m13 or 0,m14 or 0,m15 or 0,m16 or 0
end

function Matrix:Get()
    return self.m1,self.m2,self.m3,self.m4,
           self.m5,self.m6,self.m7,self.m8,
           self.m9,self.m10,self.m11,self.m12,
           self.m13,self.m14,self.m15,self.m16 
end

