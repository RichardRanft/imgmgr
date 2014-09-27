function tablelen(T)
	local count = 0;
	for _ in pairs(T) do count = count + 1 end
	return count;
end

print(" -- Creating resource manager");
ResMan = CResourceManager();
print(" -- Creating file manager");
IOMan = CLuaFileSystem();

function getDirs()
	print(" -- get directory listing");
	local list = IOMan:getDirList("");
	if tablelen(list) > 0 then
		return list;
	else
		print("   -- list length not > 0");
		return;
	end
end

function getFiles(filter)
	print(" -- get file listing " .. filter);
	local list = IOMan:getFileList(filter);
	if tablelen(list) > 0 then
		return list;
	else
		print("   -- list length not > 0");
		return;
	end
end

function getGeometry(numCells)
	local xCount = 0;
	local yCount = 0;
	local cellBy = math.sqrt(numCells);
	local temp = math.floor(cellBy);
	local test = cellBy - temp;
	if test > 0 then
		xCount = math.floor(cellBy);
		yCount = math.floor(cellBy);
	else
		xCount = math.floor(cellBy) + 1;
		yCount = math.floor(cellBy) + 1;
	end
	local geometryStr = " -geometry +" .. xCount .. "+".. yCount;
	return geometryStr;
end

function createImageList()
	local folders = getDirs();
	local files = {};
	local imageCount = 0;
	for i = 0, tablelen(folders) do
		if folders[i] ~= nil then
			images = getFiles(".\\" .. folders[i] .. "\\*.png");
			if images ~= nil and tablelen(images) > 0 then
				for j = 0, tablelen(images) do
					if images[j] ~= nil then
						files[imageCount] = ".\\" .. folders[i] .. "\\" .. images[j];
						imageCount = imageCount + 1;
					end
				end
			end
		end
	end
	return files;
end

function loadImages(list)
	for i = 0, tablelen(list) do
		ResMan:loadImage(list[i]);
	end
end

ImageList = createImageList();
print(" -- Image list length: " .. tablelen(ImageList));
loadImages(ImageList);
print(" -- determinging geometry...");
geom = getGeometry(tablelen(ImageList));
command = "montage" .. geom;
print(command);
for i = 0, tablelen(ImageList) do
	if ImageList[i] ~= nil then
		command = command .. " " .. ImageList[i];
	end
end
command = command .. " spriteSheet.png";
print(command);
os.execute(command);