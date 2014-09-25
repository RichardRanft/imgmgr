function tablelen(T)
	local count = 0;
	for _ in pairs(T) do count = count + 1 end
	return count;
end

print(" -- Creating resource manager");
ResMan = CResourceManager();
print(" -- Loading image.");
reelImage = ResMan:loadImage("reel.png");
if reelImage == nil then
	print(" -- image not loaded");
end
width, height, size = ResMan:getImageInfo("reel.png");
print(" -- image data: width " .. width .. ", height " .. height .. ", size " .. size); 