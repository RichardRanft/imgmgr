folderDiv = package.config:sub(1,1);

if folderDiv == "\\" then
	print("Windows");
else
	print("Linux/Unix");
end