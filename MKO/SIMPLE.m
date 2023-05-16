

mesh.nodes = zeros(2, 100);
mesh.cells = cell(1,100);
mesh.faces = zeros(2, 100);

mesh.center = zeros(2, 100);
mesh.volume = zeros(1, 100);
mesh.normal = zeros(2, 100);
mesh.faceCenter = zeros(2, 100);

mesh.owner = zeros(1, 100);
mesh.neighbor = zeros(1, 100);




function [center] = calculateCellCenters(mesh)

    center = mesh.center;
    i = 1;
    [~, n] = size(mesh.cells);
    while i <= n
        [~, cellSize] = size(cell2mat(mesh.cells(i)));
        center(i) = mesh.nodes(cell2mat(mesh.cells(i))) / cellSize;
        i = i + 1;
    end
end

function [volume] = calculateCellVolumes(mesh)
    
    volume = 1;
end

function [normal] = calculateFaceNormals(mesh)

    normal = [2, 1];
end

function [center] = calculateFaceCenters(mesh)

    center = [2, 1];
end




