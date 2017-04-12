function tablelen(T)
	local count = 0;
	for _ in pairs(T) do count = count + 1 end
	return count;
end

print(" -- Creating resource manager");
ResMan = CResourceManager();

local imagename = "find.png";

print(" -- Loading image " .. imagename .. ".");
findImage = ResMan:loadImage(imagename);
if findImage == nil then
	print(" -- image not loaded");
end
width, height, size = ResMan:getImageInfo(imagename);
print(" -- image data: width " .. width .. ", height " .. height .. ", size " .. size); 