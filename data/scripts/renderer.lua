require "scripts/base/class"

Renderer = Class(function(self)
    self.inst = GetGlobalRenderer()
end)

function Renderer:SetMatrix(matrixType, matrix)
    RendererSetMatrix(self.inst, matrixType, matrix:Get())
end

function Renderer:GetMatrix()
    return Matrix(RendererGetMatrix(self.inst, matrixType))
end