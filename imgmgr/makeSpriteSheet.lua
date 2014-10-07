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
	--print(" -- get directory listing");
	local list = IOMan:getDirList("");
	if tablelen(list) > 0 then
		return list;
	else
		print("   -- list length not > 0");
		return;
	end
end

function getFiles(filter)
	--print(" -- get file listing " .. filter);
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
		xCount = math.floor(cellBy) + 1;
		yCount = math.floor(cellBy) + 1;
	else
		xCount = math.floor(cellBy);
		yCount = math.floor(cellBy);
	end
	return xCount, yCount;
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
print(" -- image list length: " .. tablelen(ImageList));
loadImages(ImageList);

resdump = ResMan:dumpResourceList();
for i = 0, tablelen(resdump) do
	
end

print(" -- determining geometry...");
command = "montage -geometry +0+0";

for i = 0, tablelen(ImageList) do
	if ImageList[i] ~= nil then
		command = command .. " " .. ImageList[i];
	end
end
command = command .. " spriteSheet.png";
--print(command);
os.execute(command);

width, height = ResMan:getImageInfo(ImageList[0]);
xCells, yCells = getGeometry(tablelen(ImageList));

assetTable = {};
assetTable[1] = "<ImageAsset\n";
assetTable[2] = "    AssetName=\"spriteSheet\"\n";
assetTable[3] = "    ImageFile=\"spriteSheet.png\"\n";
assetTable[4] = "    CellCountX=\"" .. xCells .. "\"\n";
assetTable[5] = "    CellCountY=\"" .. yCells .. "\"\n";
assetTable[6] = "    CellWidth=\"" .. width .. "\"\n";
assetTable[7] = "    CellHeight=\"" .. height .. "\" />\n";

TAMLOut = io.open("spriteSheet.asset.taml", "w+");
if TAMLOut ~= nil then
	for i = 1, tablelen(assetTable) do
		TAMLOut:write(assetTable[i]);
	end
	TAMLOut:flush();
	TAMLOut:close();
end

frames = "0";
for i = 1, (tablelen(ImageList) - 1) do
	frames = frames .. " " .. i;
end

animTable = {};
animTable[1] = "<AnimationAsset\n";
animTable[2] = "    AssetName=\"spriteSheetAnim\"\n";
animTable[3] = "    Image=\"@asset=ToyAssets:spriteSheet\"\n";
animTable[4] = "    AnimationFrames=\"" .. frames .. "\"\n";
animTable[5] = "    AnimationTime=\"2\" />\n";

TAMLOut = io.open("spriteSheetAnim.asset.taml", "w+");
if TAMLOut ~= nil then
	for i = 1, tablelen(animTable) do
		TAMLOut:write(animTable[i]);
	end
	TAMLOut:flush();
	TAMLOut:close();
end

print(" -- done");