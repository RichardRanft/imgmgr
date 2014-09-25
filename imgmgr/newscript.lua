function tablelen(T)
	local count = 0;
	for _ in pairs(T) do count = count + 1 end
	return count;
end

IOMan = CLuaFileSystem();

function testDir()
	print(" -- test directory listing");
	local list = IOMan:getDirList("");
	if tablelen(list) > 0 then
		for i = 0, tablelen(list) do
			if list[i] ~= nil then
				print(" -- " .. list[i]);
			end
		end
		print("   -- dir test success");
		return;
	else
		print("   -- list length not > 0");
		return;
	end
end

function testFiles()
	print(" -- now test file listing");
	local filelist = IOMan:getFileList("*.*");
	if tablelen(filelist) > 0 then
		for i = 0, tablelen(filelist) do
			if filelist[i] ~= nil then
				print(" -- " .. filelist[i]);
			end
		end
		print("   -- file test success");
		return;
	else
		print("   -- list length not > 0");
		return;
	end
end

function testAll()
	print(" -- newscript.lua loading...");
	print("   -- test dirs");
	testDir();
	print("   -- test files");
	testFiles();
	print(" -- newscript.lua finished.");
end

testAll();