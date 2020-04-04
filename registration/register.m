function [aligned] = ...
                    depth_rgb_registration(depthData, rgbData,...
                    fx_d, fy_d, cx_d, cy_d,...
                    fx_rgb, fy_rgb, cx_rgb, cy_rgb,...
                    extrinsics)

    depthHeight = size(depthData, 1);
    depthWidth = size(depthData, 2);
    
    % Aligned will contain X, Y, Z, R, G, B values in its planes
    aligned = zeros(depthHeight, depthWidth, 6);

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

            aligned(v,u,4) = single(rgbData(y, x, 1);
            aligned(v,u,5) = single(rgbData(y, x, 2);
            aligned(v,u,6) = single(rgbData(y, x, 3);
        end
    end    
end
