import os
import cv2
import tqdm
import shutil
import numpy as np

def is_image(file):
    is_image = False
    file_ext = file.split(".")[-1]
    
    if (file_ext.lower() == "jpg") or (file_ext.lower() == "jpeg") or(file_ext.lower() == "png") or (file_ext.lower() == "bmp"):
        is_image = True
    
    return is_image

def get_image_list(data_root):
    total_files = list()
    imgs = list()
    
    for dirpath,_,filenames in os.walk(data_root):
        total_files += [dirpath.replace("\\","/")+"/"+file for file in filenames]

    for file in total_files:
        if is_image(file):
            imgs.append(file)
        else:
            continue
            
    return imgs

def main(data_root,names_file):
    with open(names_file,'r') as f:
        names = f.readlines()    
    names = [x.replace("\n","") for x in names]
    
    img_list = get_image_list(data_root)

    for _, img_path in enumerate(tqdm.tqdm(img_list, desc="progress")):
        mask_path = img_path.replace(img_path.split(".")[-1],"")+"mask"
        label_path = img_path.replace(img_path.split(".")[-1],"")+"txt"

        assert(os.path.isfile(mask_path))
        assert(os.path.isfile(label_path))

        mask = cv2.imread(mask_path)
        mask = mask[:,:,0]

        label = []
        with open(label_path,'r') as f:
            lines = f.readlines()

            for line in lines:
                index, class_name = line.replace("\n","").split(" ")

                if class_name == "Background":
                    mask[np.where(mask == int(index))] = 0
                    continue

                try:
                    names.index(class_name)
                except:
                    assert(False)

                label.append((int(index), class_name))

        label = sorted(label)
                
        new_index = 1
        for (index, class_name) in label:
            mask[np.where(mask == index)] = new_index
            new_index += 1
        
        mask[np.where(mask >= new_index)] = 0
        mask = cv2.merge((mask,mask,mask))

        shutil.move(mask_path,mask_path+".old")
        shutil.move(label_path,label_path+".old")

        new_label = open(label_path,'w')

        new_index = 1
        for (index, class_name) in label:
            new_label.write(str(new_index)+" "+class_name+"\n")
            new_index += 1
        new_label.close()
        cv2.imwrite(mask_path+".png",mask)
        shutil.move(mask_path+".png",mask_path)

if __name__ == '__main__':
    data_root = "./"
    names_file = "label.txt"
    main(data_root,names_file)