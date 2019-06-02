--
-- Adapted from GCC4 source
--

do
	local resourceIdMap = {};  -- map of resource id's we've already loaded
	
	import = function(script)
		-- If we haven't already loaded this file, try to load it now
		if (not resourceIdMap[script]) then
			if (LoadAndExecuteScriptResource(script)) then
				resourceIdMap[script] = true;
			else
				-- failed to load file through the resource system so fall back to the old method
				require(script);
			end
		end
	end
end

do
    local OldPrint = print;  -- save the old print() function
    
    print = function(text)
		if (text ~= nil) then
			Log(text);
		else
			Log("<nil>");
		end
	end
end


