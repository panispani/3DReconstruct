%% Register depth and color frames
% Loop and repeat over all
% Load depth and color frame
% Register
% Save, Same directories just prefix with reg*
disp("Starting registration");
dirs = ["../recordings/ball/scan1"]; % find with by searching for all dirs that have out.mkv
cx_d = 332.064270;
cy_d = 325.880615;
fx_d = 503.927643;
fy_d = 503.978729;
cx_rgb = 953.284424;
cy_rgb = 555.142456;
fx_rgb = 904.141541;
fy_rgb = 903.521362;

r11 = 0.9999986290931702;
r12 = -0.0012027585180476308;
r13 = -0.0011373480083420873;
r21 = 0.001307730097323656;
r22 = 0.9952529072761536;
r23 = 0.09731357544660568;
r31 = 0.0010149041190743446;
r32 = -0.09731493145227432;
r33 = 0.9952531456947327;
t1 = -0.032083917409181595;
t2 = -0.001908223726786673;
t3 = 0.004096729215234518;

extrinsics = [r11, r12, r13, t1; r21, r22, r23, t2; r31, r32, r33, t3; 0, 0, 0, 1];
anti = [r11, r21, r31, -t1; r12, r22, r32, -t2; r13, r23, r33, -t3; 0, 0, 0, 1];
% For Azure Kinect the depth camera is considered to be the world origin,
% so you have an extrinsic matrix to convert from world coordinates to each
% of the others
for d=1:length(dirs)
    dir = dirs(d);
    num = "0001"; % loop over all numbers
    colffilename = dir + "/color/color" + num + ".png";
    depthffilename = dir + "/depth/depth" + num + ".png";
    rgbData = imread(colffilename);
    depthData = imread(depthffilename);
    aligned = depth_rgb_registration2(depthData, rgbData, fx_d, fy_d, cx_d, cy_d, fx_rgb, fy_rgb, cx_rgb, cy_rgb, extrinsics);
end
disp(size(aligned));
imwrite(aligned(:,:,1:3) / 255  , 'test_d.png');
imwrite(aligned(:,:,4:6) / 255, 'test_rgb.png');

disp("DONE!");
%% Function definition
function [aligned] = ...
                    depth_rgb_registration2(depthData, rgbData,...
                    fx_d, fy_d, cx_d, cy_d,...
                    fx_rgb, fy_rgb, cx_rgb, cy_rgb,...
                    extrinsics)

    depthHeight = size(depthData, 1);
    depthWidth = size(depthData, 2);
    rgbHeight = size(rgbData, 1);
    rgbWidth = size(rgbData, 2);
    
    % Aligned will contain X, Y, Z, R, G, B values in its planes
    aligned = zeros(depthHeight, depthWidth, 6);

    depthScale = 5000; % Does Azure Kinect apply a depth scale? TODO 5000 for the 16-bit PNG files
    for v = 1 : (depthHeight)
        for u = 1 : (depthWidth)
            % Apply depth intrinsics
            z = single(depthData(v,u)) / depthScale;
            x = single((u - cx_d) * z) / fx_d;
            y = single((v - cy_d) * z) / fy_d;
            
            % Apply the extrinsics
            transformed = (extrinsics * [x;y;z;1])';
            aligned(v,u,1) = transformed(1);
            aligned(v,u,2) = transformed(2);
            aligned(v,u,3) = transformed(3);
        end
    end

    for v = 1 : (depthHeight)
        for u = 1 : (depthWidth)
            % Apply RGB intrinsics
            x = (aligned(v,u,1) * fx_rgb / aligned(v,u,3)) + cx_rgb;
            y = (aligned(v,u,2) * fy_rgb / aligned(v,u,3)) + cy_rgb;
            
            % "x" and "y" are indices into the RGB frame, but they may contain
            % invalid values (which correspond to the parts of the scene not visible
            % to the RGB camera.
            % Do we have a valid index?
            if (x > rgbWidth || y > rgbHeight ||...
                x < 1 || y < 1 ||...
                isnan(x) || isnan(y))
                continue;
            end
            % Need some kind of interpolation. I just did it the lazy way
            x = round(x);
            y = round(y);

            aligned(v,u,4) = single(rgbData(y, x, 1));
            aligned(v,u,5) = single(rgbData(y, x, 2));
            aligned(v,u,6) = single(rgbData(y, x, 3));
        end
    end    
end