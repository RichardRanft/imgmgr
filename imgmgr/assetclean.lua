-- This script is intended to remove old asset declaration syntax from
-- T2D asset files.  It scans a set of "common" folder spaces instead of 
-- just scanning recursively, so it may need some tweaking for use in
-- other projects.

function tablelen(T)
	local count = 0;
	for _ in pairs(T) do count = count + 1 end
	return count;
end

function cleanAssetFile(folder, path, filename)
	assetFileName = folder .. path .. filename;
	print(" -- cleaning " .. assetFileName);
	local textlines = {};
	i = 0;
	assetfile = io.open(assetFileName, "r");
	if assetfile == nil then
		print(" -- failed to open " .. filename);
		return;
	end
	for line in assetfile:lines() do
		-- remove old @assetFile declaration syntax
		textlines[i] = string.gsub(line, "()@assetFile=#()", "");
		i = i + 1;
	end
	assetfile:close();
	assetfile = io.open(assetFileName, "w+");
	for j = 0, i do
		if textlines[j] ~= nil then
			assetfile:write(textlines[j] .. "\n");
		print(textlines[j]);
		end
	end
	assetfile:close();
end

IOMan = CLuaFileSystem();

local list = IOMan:getDirList("");
if tablelen(list) > 0 then
	for i = 0, tablelen(list) do
		if list[i] ~= nil then
			print(list[i]);
		end
	end
else
	print(" -- list length not > 0");
	return;
end

for i = 0, tablelen(list) do
	print(" -- Start cleaning....");
	local folder = list[i];
	assetPath = "\\1\\data\\audio\\";
	local filelist1 = IOMan:getFileList(".\\" .. folder .. assetPath .. "*.asset.taml");
	assetPath = "\\1\\data\\images\\";
	local filelist2 = IOMan:getFileList(".\\" .. folder .. assetPath .. "*.asset.taml");
	assetPath = "\\1\\assets\\audio\\";
	local filelist3 = IOMan:getFileList(".\\" .. folder .. assetPath .. "*.asset.taml");
	assetPath = "\\1\\assets\\images\\";
	local filelist4 = IOMan:getFileList(".\\" .. folder .. assetPath .. "*.asset.taml");
	assetPath = "\\1\\audio\\";
	local filelist5 = IOMan:getFileList(".\\" .. folder .. assetPath .. "*.asset.taml");
	assetPath = "\\1\\images\\";
	local filelist6 = IOMan:getFileList(".\\" .. folder .. assetPath .. "*.asset.taml");
	
	if tablelen(filelist1) > 0 then
		for j = 0, tablelen(filelist1) do
			if filelist1[j] ~= nil then
				assetPath = "\\1\\data\\audio\\";
				print(" -- path: " .. assetPath);
				cleanAssetFile(folder, assetPath, filelist1[j]);
			end
		end
	end
	if tablelen(filelist2) > 0 then
		for j = 0, tablelen(filelist2) do
			if filelist2[j] ~= nil then
				assetPath = "\\1\\data\\images\\";
				print(" -- path: " .. assetPath);
				cleanAssetFile(folder, assetPath, filelist2[j]);
			end
		end
	end
	if tablelen(filelist3) > 0 then
		for j = 0, tablelen(filelist3) do
			if filelist3[j] ~= nil then
				assetPath = "\\1\\assets\\audio\\";
				print(" -- path: " .. assetPath);
				cleanAssetFile(folder, assetPath, filelist3[j]);
			end
		end
	end
	if tablelen(filelist4) > 0 then
		for j = 0, tablelen(filelist4) do
			if filelist4[j] ~= nil then
				assetPath = "\\1\\assets\\images\\";
				print(" -- path: " .. assetPath);
				cleanAssetFile(folder, assetPath, filelist4[j]);
			end
		end
	end
	if tablelen(filelist5) > 0 then
		for j = 0, tablelen(filelist5) do
			if filelist5[j] ~= nil then
				assetPath = "\\1\\audio\\";
				print(" -- path: " .. assetPath);
				cleanAssetFile(folder, assetPath, filelist5[j]);
			end
		end
	end
	if tablelen(filelist6) > 0 then
		for j = 0, tablelen(filelist6) do
			if filelist6[j] ~= nil then
				assetPath = "\\1\\images\\";
				print(" -- path: " .. assetPath);
				cleanAssetFile(folder, assetPath, filelist6[j]);
			end
		end
	end
	print(" -- finished!");
end

