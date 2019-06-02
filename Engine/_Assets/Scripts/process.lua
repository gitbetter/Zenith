TestScriptProcess = ZScriptableProcess;

function TestScriptProcess:OnInitialize()
	print("Initializing...");
	self.count = 0;
end

function TestScriptProcess:OnUpdate()
	self.count = self.count + 1;
	print("Count: " .. self.count);
	if (self.count >= 10) then
		self:Finish();
	end
end

function TestScriptProcess:OnFinish()
	print("Done!");
end

parent = TestScriptProcess:Create({frequency = 1000.0});
child = TestScriptProcess:Create({frequency = 500.0});
parent:AttachChild(child);
AttachProcess(parent);