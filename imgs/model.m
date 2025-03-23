clear; clc;

number = '%RH';
max_width_bytes = 6;

%% Convert bitmap to binary code

PIC = imread([number,'.bmp']);
PIC = 255 - PIC;

m = size(PIC, 1);
n = size(PIC, 2);

outmat = uint8(zeros(m, ceil(n/8)));

for i = 1:1:m

    linedata = PIC(i, :);
    colcnt = 1;
    j = 1;

    while j <= n

        mask = uint8(128);
        buf = 0;

        for k = 1:1:8

            if linedata(j) > 0
                buf = buf + mask;
            end
            mask = mask / 2;
            j = j + 1;

            if j > n
                break;
            end
        end

        outmat(i, colcnt) = buf;
        colcnt = colcnt + 1;
    end
end

%% Generate C Code Print

clc;
m = size(outmat, 1);
n = size(outmat, 2);

for i = 1:1:m
    linedata = outmat(i,:);
    for j = 1:1:max_width_bytes
        if j > n
            fprintf(' 0,');
        else
            fprintf(' %d,', linedata(j));
        end
    end
    fprintf('\n');
end
