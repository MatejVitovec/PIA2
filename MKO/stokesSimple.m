clear all


% mesh = loadGMSH("cavita.msh");

mesh = cartesianMesh(25, 1/25);

uBc = {};
pBc = {};

uBc = setDirichletBoundaryCondition([1,0], "top", uBc, mesh);
uBc = setDirichletBoundaryCondition([0,0], "bottom", uBc, mesh);
uBc = setDirichletBoundaryCondition([0,0], "left", uBc, mesh);
uBc = setDirichletBoundaryCondition([0,0], "right", uBc, mesh);

pBc = setNeumannBoundaryCondition(1, "top", pBc, mesh);
pBc = setNeumannBoundaryCondition(1, "bottom", pBc, mesh);
pBc = setNeumannBoundaryCondition(1, "left", pBc, mesh);
pBc = setNeumannBoundaryCondition(1, "right", pBc, mesh);

u0 = zeros(length(mesh.cells), 2);
p0 = zeros(length(mesh.cells), 1);
mu = ones(length(mesh.cells), 1)*1.e-4;
% v = ones(length(mesh.cells), 1)*0.01;

alpha = 0.7;
beta = 0.3;

[u, p, res] = SIMPLE(u0, p0, mu, mesh, uBc, pBc, alpha, beta, 50);
% [u, p, res] = SIMPLENS(u0, p0, v, mesh, uBc, pBc, alpha, beta, 50);


figure
hold on
plotPressure(p, mesh);
plotVelocity(u, mesh)
hold off

figure
plotRes(res);

% saveVTK("output.vtk", mesh, u, p);





function [u, p, res] = SIMPLE(u0, p0, mu, mesh, uBc, pBc, alpha, beta, iteration)
    
    u = u0;
    p = p0;
    res = zeros(2);

    for iter = 0:iteration
        uOld = u;
        pOld = p;
    
        uEqn = minusEquation(laplace(mu, u, mesh, uBc));
        
        uEqn = relax(uEqn, u, alpha);
        
        u = solve(addVectorToEquation(uEqn, grad(p, mesh, pBc)));
        
        ra = 1./ac(uEqn);
        
        ubar = ra.*H(uEqn, u);
        
        pEqn = addVectorToEquation(laplace(ra, p, mesh, pBc), -div(ubar, mesh, uBc));
        pEqn.A(1,1) = pEqn.A(1,1) - alpha/mu(1);
        
        p = solve(pEqn);
        
        p = beta*p + (1 - beta)*pOld;
        u = ubar - ra.*grad(p, mesh, pBc);

        res(iter + 1, 1) = norm(uOld - u);
        res(iter + 1, 2) = norm(pOld - p);
    
        if(rem(iter, 5) == 0)
            disp("iteration: " + iter);
        end    
    end
end


% ---------------Equation functions----------------------

function eqn = addEquations(eqn1, varargin)
    eqn.A = eqn1.A;
    eqn.b = eqn1.b;

    for q = varargin
        eqn.A = eqn.A + (cell2mat(q).A);
        eqn.b = eqn.b + (cell2mat(q).b);
    end
end

function eqn = addVectorToEquation(eqn1, varargin)
    eqn.A = eqn1.A;
    eqn.b = eqn1.b;

    for q = varargin
        eqn.b = eqn.b + cell2mat(q);
    end
end

function eqn = minusEquation(eqn1)
    eqn.A = -eqn1.A;
    eqn.b = -eqn1.b;
end

function x = solve(eqn)
    x = eqn.A \ eqn.b;
    x = -x;
end

function eqn = relax(eqn, u, alpha)
    d = diag(eqn.A);
    for i = 1:length(d)
        eqn.A(i,i) = eqn.A(i,i)/alpha;
    end
    eqn.b = eqn.b - (1 - alpha)/alpha*d.*u;
end

function out = ac(eqn)
    out = diag(eqn.A);
end

function out = H(eqn, x)
    eqn.A = -((eqn.A).*~eye(size(eqn.A)));
    out = eqn.A*x - eqn.b;
end

function eqn = laplace(mu, u, mesh, bc)
    size = length(mesh.cells);
    eqn.A = zeros(size, size);
    eqn.b = zeros(size, length(u(1,:)));

    for f = 1:length(mesh.faces)
        if mesh.neighbor(f) == 0
            continue
        end
        o = mesh.owner(f);
        n = mesh.neighbor(f);
        xo = mesh.center(o,:);
        xn = mesh.center(n,:);
        S = mesh.normal(f,:);
        
        muf = (mu(o) + mu(n))/2;
        g = muf*(norm(S)/norm(xn - xo));
        
        eqn.A(o,o) = eqn.A(o,o) - g/mesh.volume(o);
        eqn.A(o,n) = eqn.A(o,n) + g/mesh.volume(o);
            
        eqn.A(n,o) = eqn.A(n,o) + g/mesh.volume(n);
        eqn.A(n,n) = eqn.A(n,n) - g/mesh.volume(n);
    end

    for j = 1:length(mesh.boundaryNames)
        idx = 1;
        for f = cell2mat(mesh.boundaryFaces(j))'
            o = mesh.owner(f);
            S = mesh.normal(f,:);
            xo = mesh.center(o,:);
            xf = mesh.faceCenter(f,:);
            c1 = bc(j,idx).c1;
            c2 = bc(j,idx).c2;
            
            delta = dot(xf - xo, S)/norm(S);
            muf = mu(o);

            eqn.A(o,o) = eqn.A(o,o) + muf*((c2 - 1)/delta)*(norm(S)/mesh.volume(o));
            eqn.b(o,:) = eqn.b(o,:) + muf*(c1/delta)*(norm(S)/mesh.volume(o));
            idx = idx + 1;
        end
    end
end

function out = grad(u, mesh, bc)
    out = zeros(length(mesh.cells), 2);
    
    for f = 1:length(mesh.faces)
        if mesh.neighbor(f) == 0
            continue
        end
        o = mesh.owner(f);
        n = mesh.neighbor(f);
        uf = (u(o) + u(n))/2.0;
        flux = uf*mesh.normal(f,:);
        out(o,:) = out(o,:) + flux;
        out(n,:) = out(n,:) - flux;
    end

    for j = 1:length(mesh.boundaryNames)
        idx = 1;
        for f = cell2mat(mesh.boundaryFaces(j))'
            o = mesh.owner(f);
            c1 = bc(j,idx).c1;
            c2 = bc(j,idx).c2;

            ub = c1 + c2*u(o,:);
            flux = ub*mesh.normal(f,:);
            
            out(o,:) = out(o,:) + flux;
            idx = idx + 1;
        end
    end

    for c = 1:length(mesh.cells)
        out(c,:) = out(c,:) / mesh.volume(c);
    end
end

function out = div(u, mesh, bc)
    out = zeros(length(mesh.cells), 1);
    
    for f = 1:length(mesh.faces)
        if mesh.neighbor(f) == 0
            continue
        end
        o = mesh.owner(f);
        n = mesh.neighbor(f);
        uf = (u(o,:) + u(n,:))/2.0;
        flux = dot(uf, mesh.normal(f,:));
        out(o) = out(o) + flux;
        out(n) = out(n) - flux;
    end

    for j = 1:length(mesh.boundaryNames)
        idx = 1;
        for f = cell2mat(mesh.boundaryFaces(j))'
            o = mesh.owner(f);
            c1 = bc(j,idx).c1;
            c2 = bc(j,idx).c2;

            ub = c1 + c2*u(o,:);
            flux = dot(ub, mesh.normal(f,:));
            
            out(o) = out(o) + flux;
            idx = idx + 1;
        end
    end

    for c = 1:length(mesh.cells)
        out(c) = out(c) / mesh.volume(c);
    end
end



% ---------------END Equation functions------------------




% ---------------Boundary conditions---------------------

function boundaryConditions = setDirichletBoundaryCondition(value, name, boundaryConditions, mesh)
    idx = find(strcmp(mesh.boundaryNames, name));    
    for i = 1:length(cell2mat(mesh.boundaryFaces(idx)))
        boundaryConditions(idx, i).c1 = value;
        boundaryConditions(idx, i).c2 = 0;
    end
end

function boundaryConditions = setNeumannBoundaryCondition(value, name, boundaryConditions, mesh)
    idx = find(strcmp(mesh.boundaryNames, name));
%     TODO c1 for vector field
    for i = 1:length(cell2mat(mesh.boundaryFaces(idx)))
        boundaryConditions(idx, i).c1 = 0;
        boundaryConditions(idx, i).c2 = value;
    end
end

% ---------------END Boundary conditions-----------------




% ---------------Mesh functions--------------------------

function [faces, owner, neighbor] = createFaces(mesh)
    faces = double.empty(0,2);
    owner = [];
    neighbor = [];
    for c = 1:length(mesh.cells)
        cell = cell2mat(mesh.cells(c));
        for f = 1:length(cell)
            secondIdx = f+1;
            if(f == length(cell))
                secondIdx = 1;
            end
            face = [cell(f), cell(secondIdx)];

            a = ismember(faces,face,'rows');
            b = ismember(faces,flip(face),'rows');

            if ~(sum(a) || sum(b))
                owner(end+1) = c;
                faces(end+1,:) = face;
            else
                idx = find(a|b);
                neighbor(idx) = c;
            end
        end
    end
    neighbor(numel(owner)) = 0;
end

function boundaryFaces = findBoundaryFaces(mesh, boundaryFacesNodes)
    boundaryFaces = {};

    for p = 1:length(boundaryFacesNodes)
        facesIdx = cell2mat(boundaryFacesNodes(p));
        auxList = zeros(length(facesIdx),1);
        for i = 1:length(facesIdx)
            a = ismember(mesh.faces, facesIdx(i,:),'rows');
            b = ismember(mesh.faces, flip(facesIdx(i,:)),'rows');
            auxList(i,1) = find(a|b);
        end
        boundaryFaces(p,:) = {auxList};
    end
end

function center = calculateCellCenters(mesh)
    center = zeros(length(mesh.cells),2);
    for i = 1:length(mesh.cells)
        center(i,:) = sum(mesh.nodes(cell2mat(mesh.cells(i)), :)) / length(cell2mat(mesh.cells(i)));
    end
end

function volume = calculateCellVolumes(mesh)
    volume = zeros(length(mesh.cells), 1);
    for i = 1:length(mesh.cells)
        cellIndex = cell2mat(mesh.cells(i));
        x = mesh.nodes(cellIndex,1);
        y = mesh.nodes(cellIndex,2);
        volume(i) = sum(x.*circshift(y,-1) - circshift(x,-1).*y)/2;
    end
end

function normal = calculateFaceNormals(mesh)
    normal = fliplr(mesh.nodes(mesh.faces(:,2), :) - mesh.nodes(mesh.faces(:,1), :)).*[1 ,-1];
end

function center = calculateFaceCenters(mesh)
    center = (mesh.nodes(mesh.faces(:,1), :) + mesh.nodes(mesh.faces(:,2), :))/2;
end

function mesh = cartesianMesh(size, h)
    nSize = size + 1;
    mesh.nodes = zeros(nSize*nSize, 2);
    mesh.cells = {};

    for m = 1:nSize
        for n = 1:nSize
            mesh.nodes((m-1)*nSize + n, :) = [(n-1)*h, (m-1)*h];
        end
    end
    for i = 1:size*size
        mesh.cells(i, :) = {[i+floor((i-1)/size), i+floor((i-1)/size+1), i+floor((i-1)/size+1)+size+1, i+floor((i-1)/size+1)+size]};
    end

    mesh.boundaryNames = ["bottom", "right", "top", "left"];
    boundaryFacesNodes = {};
    aux = zeros(size, 2);
    for i = 1:size
        aux(i,:) = [i, i+1];
    end
    boundaryFacesNodes(1,:) = {aux};

    aux = zeros(size, 2);
    for i = 1:size
        aux(i,:) = [(size+1)*i, (size+1)*(i+1)];
    end
    boundaryFacesNodes(2,:) = {aux};

    aux = zeros(size, 2);
    for i = 1:size
        aux(i,:) = [size*(size+1)+i, size*(size+1)+i+1];        
    end
    boundaryFacesNodes(3,:) = {aux};

    aux = zeros(size, 2);
    for i = 1:size
        aux(i,:) = [(size+1)*(i-1)+1, (size+1)*i+1];
    end
    boundaryFacesNodes(4,:) = {aux};

    mesh = createMesh(mesh, boundaryFacesNodes);
end

function mesh = loadGMSH(fileName)
    textData = string(splitlines(fileread(fileName)));

    physicalNamesStart = find(strcmp(textData, "$PhysicalNames"));
    physicalNamesEnd = find(strcmp(textData, "$EndPhysicalNames"));

    nodesStart = find(strcmp(textData, "$Nodes"));
    nodesEnd = find(strcmp(textData, "$EndNodes"));

    elementsStart = find(strcmp(textData, "$Elements"));
    elementsEnd = find(strcmp(textData, "$EndElements"));

    mesh.boundaryNames = ["first", "second"];
    physicalNamesId = [0,0];
    inc = 1;
    for i = physicalNamesStart+2:physicalNamesEnd-1
        aux = split(textData(i,:));
        if(str2double(aux(1)) ~= 1)
            inc = inc + 1;
            continue;
        end
        physicalNamesId(inc) = str2double(aux(2));
        name = split(aux(3), '"');
        name = name(2);
        mesh.boundaryNames(inc) = name;
        inc = inc + 1;
    end

    mesh.nodes = zeros(2);
    for i = nodesStart+2:nodesEnd-1
        node = split(textData(i,:));
        inc = str2double(node(1));
        mesh.nodes(inc,1) = node(2);
        mesh.nodes(inc,2) = node(3);
    end

    mesh.cells = {};
    inc = 1;
    for i = elementsStart+2:elementsEnd-1
        cell = str2double(split(textData(i,:)));

        if(~(cell(2) == 2 || cell(2) == 3))
            continue;
        end

        tags = cell(3);
        firstIdx = 4 + tags;
        nodesSize = length(cell) - (firstIdx-1);

        if(nodesSize == 3)
            mesh.cells(inc, :) = {[cell(firstIdx), cell(firstIdx+1), cell(firstIdx+2)]};
        elseif(nodesSize == 4)
            mesh.cells(inc, :) = {[cell(firstIdx), cell(firstIdx+1), cell(firstIdx+2), cell(firstIdx+3)]};
        end
        inc = inc + 1;
    end

    boundaryFacesNodes = {};
    for j = 1:length(mesh.boundaryNames)
        aux = zeros(2);
        inc = 1;
        for i = elementsStart+2:elementsEnd-1
            cell = str2double(split(textData(i,:)));

            if(~(cell(4) == physicalNamesId(j)))
                continue;
            end
            
            if(~(cell(2) == 1))
                continue;
            end

            tags = cell(3);
            firstIdx = 4 + tags;

            aux(inc,:) = [cell(firstIdx), cell(firstIdx + 1)];
            inc = inc + 1;
        end
        boundaryFacesNodes(j,:) = {aux};
    end

    mesh = createMesh(mesh, boundaryFacesNodes);
end

function mesh = createMesh(mesh, boundaryFacesNodes)

    [mesh.faces, mesh.owner, mesh.neighbor] = createFaces(mesh);

    mesh.boundaryFaces = findBoundaryFaces(mesh, boundaryFacesNodes);
    
    mesh.center = calculateCellCenters(mesh);
    mesh.volume = calculateCellVolumes(mesh);
    mesh.faceCenter = calculateFaceCenters(mesh);
    mesh.normal = calculateFaceNormals(mesh);
end

function plotMesh(mesh)
    
    for i = 1:length(mesh.faces)
        plot(mesh.nodes(mesh.faces(i,:), 1), mesh.nodes(mesh.faces(i,:), 2), "k-");
    end
end

function plotFaceNormals(mesh)
    for i = 1:length(mesh.faces)
        plot([mesh.faceCenter(i,1), mesh.faceCenter(i,1)+mesh.normal(i,1)], [mesh.faceCenter(i,2), mesh.faceCenter(i,2)+mesh.normal(i,2)], "r-");
    end
end

function plotCellCenter(mesh)
    for i = 1:length(mesh.center)
        plot(mesh.center(i,1),mesh.center(i,2), "g*");
    end
end


% ---------------END Mesh functions--------------------------




% ---------------Data vizualization--------------------------

function plotPressure(p, mesh)
    x = mesh.center(:,1);
    y = mesh.center(:,2);

    xMin = min(mesh.nodes(:,1));
    xMax = max(mesh.nodes(:,1));
    yMin = min(mesh.nodes(:,2));
    yMax = max(mesh.nodes(:,2));

    F = scatteredInterpolant(x, y, p);
    [xq, yq] = meshgrid(xMin:(xMax-xMin)/100:xMax, yMin:(yMax-yMin)/100:yMax);
    v = F(xq, yq);

    contourf(xq, yq, v);
    colorbar;
end

function plotVelocity(u, mesh)
    drawArrow = @(x,y,u,v) quiver(x, y, u, v, 0, "k");

    x = mesh.center(:,1);
    y = mesh.center(:,2);

    maxNormU = max(sqrt(sum(abs(u).^2,2)));
    scale = min(max(mesh.center(:,1)) - min(mesh.center(:,1)), max(mesh.center(:,2)) - min(mesh.center(:,2))) * 10;

    uu = u/(maxNormU*scale);

    hold on
    for i = 1:length(u)
        drawArrow(x,y,uu(:,1), uu(:,2));
    end
end

function plotRes(res)
    hold on

    uRes = res(:,1);
    pRes = res(:,2);
    iter = length(uRes);

    subplot(1,2,1);
    plot(1:iter, uRes);
    ylabel("norm(u)");
    xlabel("iteratio");

    subplot(1,2,2)
    plot(1:iter, pRes);
    ylabel("norm(p)");
    xlabel("iteratio");
end

function saveVTK(fName, mesh, u, p)
    fileID = fopen(fName,'w');

    fprintf(fileID, "# vtk DataFile Version 1.0\n");
    fprintf(fileID, "unstructured grid\n");
    fprintf(fileID, "ascii\n");
    fprintf(fileID, "DATASET UNSTRUCTURED_GRID\n");

    fprintf(fileID, "points " + length(mesh.nodes) + " float\n");
    for i = 1:length(mesh.nodes)
        fprintf(fileID, mesh.nodes(i,1) + " " + mesh.nodes(i,2) + " 0 \n");
    end

    num = 0;
    for i = 1:length(mesh.cells)
        num = num + 1 + length(cell2mat(mesh.cells(i)));
    end

    fprintf(fileID, "cells " + length(mesh.cells) + " " + num + "\n");
    for i = 1:length(mesh.cells)
        c = cell2mat(mesh.cells(i));
        fprintf(fileID, length(c) + " ");        
        for j = 1:length(c)
            fprintf(fileID, c(j) - 1 + " ");
        end
        fprintf(fileID, "\n");
    end

    fprintf(fileID, "cell_types " + length(mesh.cells) + "\n");
    for i = 1:length(mesh.cells)
        fprintf(fileID, "7\n");
    end

    fprintf(fileID, "CELL_DATA " + length(mesh.cells) + "\n");
    fprintf(fileID, "SCALARS p float\n");
    fprintf(fileID, "LOOKUP_TABLE default\n");
    for i = 1:length(mesh.cells)
        pp = p(i);
        if(pp < 10e-12 && pp > -10e-12)
            pp = 0;
        end
        fprintf(fileID, pp + "\n");
    end

    fprintf(fileID, "SCALARS u float 3\n");
    fprintf(fileID, "LOOKUP_TABLE default\n");
    for i = 1:length(mesh.cells)
        uu = u(i,:);
        if(uu(1) < 10e-12 && uu(1) > -10e-12)
            uu(1) = 0;
        end
        if(uu(2) < 10e-12 && uu(2) > -10e-12)
            uu(2) = 0;
        end
        fprintf(fileID, uu(1) + " " + uu(2) + " 0\n");
    end

    fclose(fileID);
end





% ---------------Navier Stokes--------------------------


function phi = phiInterpolation(u, mesh, bc)
    phi = zeros(length(mesh.faces), 1);
    
    for f = 1:length(mesh.faces)
        if mesh.neighbor(f) == 0
            continue
        end
        o = mesh.owner(f);
        n = mesh.neighbor(f);
        phi(f,:) = dot(mesh.normal(f,:), (u(o,:) + u(n,:)) / 2);
    end

    for j = 1:length(mesh.boundaryNames)
        idx = 1;
        for f = cell2mat(mesh.boundaryFaces(j))'
            o = mesh.owner(f);
            c1 = bc(j,idx).c1;
            c2 = bc(j,idx).c2;

            ub = c1 + c2*u(o,:);
            phi(f,:) = dot(mesh.normal(f,:), ub);
            
            idx = idx + 1;
        end
    end
end



function eqn = divPhi(phi, u, mesh, bc)
    eqn.A = zeros(length(u));
    eqn.b = zeros(length(u), 2);

    for f = 1:length(mesh.faces)
        if mesh.neighbor(f) == 0
            continue
        end
        o = mesh.owner(f);
        n = mesh.neighbor(f);
        phi(f,:) = dot(mesh.normal(f,:), (u(o,:) + u(n,:)) / 2);

        phiPlus = max(phi(f,:), 0);
        phiMinus = min(phi(f,:), 0);
        
        eqn.A(o,o) = eqn.A(o,o) + phiPlus / mesh.volume(o);
        eqn.A(o,n) = eqn.A(o,n) + phiMinus / mesh.volume(o);
            
        eqn.A(n,o) = eqn.A(n,o) - phiPlus / mesh.volume(n);
        eqn.A(n,n) = eqn.A(n,n) - phiMinus / mesh.volume(n);
    end

    for j = 1:length(mesh.boundaryNames)
        idx = 1;
        for f = cell2mat(mesh.boundaryFaces(j))'
            o = mesh.owner(f);
            c1 = bc(j,idx).c1;
            c2 = bc(j,idx).c2;
            phiPlus = max(phi(f,:), 0);
            phiMinus = min(phi(f,:), 0);

            eqn.A(o,o) = eqn.A(o,o) + (phiPlus + phiMinus*c2) / mesh.volume(o);
            eqn.b(o,:) = eqn.b(o,:) + phiMinus*c1 / mesh.volume(o);
            
            idx = idx + 1;
        end
    end
end



function phi = calcPhi(phi, u, p, ra, mesh, uBc, pBc)

    for f = 1:length(mesh.faces)
        if mesh.neighbor(f) == 0
            continue
        end
        o = mesh.owner(f);
        n = mesh.neighbor(f);
        delta = norm(mesh.center(n,:) - mesh.center(o,:));
        pn = (p(n) - p(o)) / delta;

        phi(f,:) = dot(mesh.normal(f,:), (u(o,:) + u(n,:))/2 - (ra(o) + ra(n))/2*pn*norm(mesh.normal(f,:)));
    end

    for j = 1:length(mesh.boundaryNames)
        idx = 1;
        for f = cell2mat(mesh.boundaryFaces(j))'
            o = mesh.owner(f);

            c1 = uBc(j,idx).c1;
            c2 = uBc(j,idx).c2;
            ub = c1 + c2*u(o,:);

            c1p = pBc(j,idx).c1;
            c2p = pBc(j,idx).c2;
            pb = c1p + c2p*p(o,:);

            pn = (pb - p(o,:))/norm(mesh.center(o,:) - mesh.faceCenter(f,:));

            phi(f,:) = dot(mesh.normal(f,:), ub) - ra(o)*pn;
            
            idx = idx + 1;
        end
    end
end


function [u, p, res] = SIMPLENS(u0, p0, v, mesh, uBc, pBc, alpha, beta, iteration)
    
    u = u0;
    p = p0;
    phi = phiInterpolation(u, mesh, uBc);

    res = zeros(2);

    for iter = 0:iteration
        uOld = u;
        pOld = p;
    
        uEqn = addEquations(divPhi(phi, u, mesh, uBc), minusEquation(laplace(v, u, mesh, uBc)));
        
        uEqn = relax(uEqn, u, alpha);
        
        u = solve(addVectorToEquation(uEqn, grad(p, mesh, pBc)));
        
        ra = 1./ac(uEqn);
        
        ubar = ra.*H(uEqn, u);
        
        pEqn = addVectorToEquation(laplace(ra, p, mesh, pBc), -div(ubar, mesh, uBc));
        pEqn.A(1,1) = pEqn.A(1,1) - alpha/v(1);
        
        p = solve(pEqn);
        
        p = beta*p + (1 - beta)*pOld;
        u = ubar - ra.*grad(p, mesh, pBc);

        phi = calcPhi(phi, u, p, ra, mesh, uBc, pBc);
    
        res(iter + 1, 1) = norm(uOld - u);
        res(iter + 1, 2) = norm(pOld - p);
    
        if(rem(iter, 5) == 0)
            disp("iteration: " + iter);
        end  
    end
end
