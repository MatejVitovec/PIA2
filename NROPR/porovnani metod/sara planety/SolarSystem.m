clear all

global masses
global G
global numRows
global numCols

data = fileread("data.txt");
data = split(splitlines(data));
[numRows,numCols] = size(data);

names = string(data(2:numRows, 2));
initialCondition = str2double(data(2:numRows, 3:8));
masses = str2double(data(2:numRows, 9));
planetNum = numRows - 1;

G = 6.67e-11;

tspan  = [0, 5000000000];
[t, y] = ode78(@F, tspan, reshape(initialCondition',[],1));

[yRows,yCols] = size(y);

out = reshape(y, yRows, 6, planetNum);

A(:,:) = out(yRows, :, :);

zeme(:,:) = out(:, :, 4);

figure
hold on

plot(out(:, 1, 1), out(:, 2, 1));
plot(out(:, 1, 2), out(:, 2, 2));
plot(out(:, 1, 3), out(:, 2, 3));
plot(out(:, 1, 4), out(:, 2, 4));
plot(out(:, 1, 5), out(:, 2, 5));
plot(out(:, 1, 6), out(:, 2, 6));
plot(out(:, 1, 7), out(:, 2, 7));
plot(out(:, 1, 8), out(:, 2, 8));
plot(out(:, 1, 9), out(:, 2, 9));
plot(out(:, 1, 10), out(:, 2, 10));

% plot(initialCondition(1, 1), initialCondition(1, 2), "+");
% plot(initialCondition(2, 1), initialCondition(2, 2), "+");
% plot(initialCondition(3, 1), initialCondition(3, 2), "+");
% plot(initialCondition(4, 1), initialCondition(4, 2), "+");
% plot(initialCondition(5, 1), initialCondition(5, 2), "+");
% plot(initialCondition(6, 1), initialCondition(6, 2), "+");
% plot(initialCondition(7, 1), initialCondition(7, 2), "+");
% plot(initialCondition(8, 1), initialCondition(8, 2), "+");
% plot(initialCondition(9, 1), initialCondition(9, 2), "+");
% plot(initialCondition(10, 1), initialCondition(10, 2), "+");

legend(names);



function [f] = F(~, u)
    global masses
    global G
    global numRows
    global numCols

    planetNum = numRows - 1;

    f = zeros(planetNum * 6, 1);

    index = 1;

    while index <= planetNum
    
        f(1+(index-1)*6) = u(4+(index-1)*6);
        f(2+(index-1)*6) = u(5+(index-1)*6);
        f(3+(index-1)*6) = u(6+(index-1)*6);
        f(4+(index-1)*6) = 0;
        f(5+(index-1)*6) = 0;
        f(6+(index-1)*6) = 0;
    
        i = 1;
        while i <= planetNum
            if i == index
                i = i + 1;
                continue;
            end
            dist = distance(u((1+(i-1)*6):(3+(i-1)*6)), u((1+(index-1)*6):(3+(index-1)*6)));
            distance3 = dist^3;
            f(4+(index-1)*6) = f(4+(index-1)*6) + G*masses(i)*((u(1+(i-1)*6) - u(1+(index-1)*6))/distance3);
            f(5+(index-1)*6) = f(5+(index-1)*6) + G*masses(i)*((u(2+(i-1)*6) - u(2+(index-1)*6))/distance3);
            f(6+(index-1)*6) = f(6+(index-1)*6) + G*masses(i)*((u(3+(i-1)*6) - u(3+(index-1)*6))/distance3);
            i = i + 1;
        end
        index = index + 1;
    end
end

function [d] = distance(u1, u2)
    d = sqrt((u1(1) - u2(1))^2 + (u1(2) - u2(2))^2 + (u1(3) - u2(3))^2);
end




% zaloha
% function [f] = F(~, u)
%     global masses
%     global G
%     f = zeros(11, 10);
%     planetNum = 10;
% 
%     index = 1;
% 
%     while index <= planetNum
%     
%         f(1,index) = u(4, index);
%         f(2,index) = u(5, index);
%         f(3,index) = u(6, index);
%         f(4,index) = 0;
%         f(5,index) = 0;
%         f(6,index) = 0;
%     
%         i = 1;
%         while i <= planetNum
%             if i == index
%                 index = index + 1;
%                 continue;
%             end
%             f(4,index) = f(4,index) + G*masses(i)*((u(1, i) - u(1, index))/distance(u(:, i), u(:, index)));
%             f(5,index) = f(5,index) + G*masses(i)*((u(2, i) - u(2, index))/distance(u(:, i), u(:, index)));
%             f(6,index) = f(6,index) + G*masses(i)*((u(3, i) - u(3, index))/distance(u(:, i), u(:, index)));
%             i = i + 1;
%         end
%         index = index + 1;
%     end
% 
%     f = f(:);
% end
