#!/bin/env/python3


import argparse
import os

import struct
import numpy as np

def load_base(path):

    # Example: path = 'vectors.bin'
    base_vec_f = open(path, 'rb')

    base_vec_count = struct.unpack('i', base_vec_f.read(4))[0]
    base_vec_dimension = struct.unpack('i', base_vec_f.read(4))[0]

    base_vec = np.frombuffer(
        base_vec_f.read(base_vec_count * base_vec_dimension), dtype=np.int8).reshape((base_vec_count, base_vec_dimension)
        )

    # Count read file size:
    print(f"Base vector file size: {os.path.getsize(path)}")

    print(f"base_vec_count: {base_vec_count}")
    print(f"base_vec_dimension: {base_vec_dimension}")

    print(f"Base vector shape: {base_vec.shape}")

    base_vec_f.close()

    return base_vec


def load_query(path):

    # Example: path = 'vectors.bin'
    query_vec_f = open(path, 'rb')

    query_vec_count = struct.unpack('i', query_vec_f.read(4))[0]
    query_vec_dimension = struct.unpack('i', query_vec_f.read(4))[0]

    query_vec = np.frombuffer(
        query_vec_f.read(query_vec_count * query_vec_dimension), dtype=np.int32).reshape((query_vec_count, query_vec_dimension)
        )

    print(f"Query vector shape: {query_vec.shape}")

    query_vec_f.close()

    return query_vec


def load_gt(path):

    # Example: path = 'vectors.bin'
    gt_vec_f = open(path, 'rb')

    gt_vec_count = struct.unpack('i', gt_vec_f.read(4))[0]
    gt_vec_topk = struct.unpack('i', gt_vec_f.read(4))[0]

    gt_vec = np.frombuffer(
        gt_vec_f.read(gt_vec_count * gt_vec_topk * 4), dtype=np.int32).reshape((gt_vec_count, gt_vec_topk)
        )

    gt_distance_list = np.frombuffer(
        gt_vec_f.read(gt_vec_count * gt_vec_topk * 4), dtype=np.float32).reshape((gt_vec_count, gt_vec_topk)
        )
    
    print(f"Ground Truth shape: {gt_vec.shape}")

    gt_vec_f.close()
  
    return gt_vec, gt_distance_list


if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    parser.add_argument('--path-base', type=str, default="", help='Base vector file path.')
    parser.add_argument('--path-query', type=str, default="", help='Query vector file path.')
    parser.add_argument('--path-gt', type=str, default="", help='Ground truth vector file path.')

    args = parser.parse_args()

    # 
    # Load base vectors
    np_base_vec = load_base(args.path_base)
    np.save(args.path_base + '.npy', np_base_vec)

    np_query_vec = load_query(args.path_query)
    np.save(args.path_query + '.npy', np_query_vec)

    np_gt_vec, np_gt_dist = load_gt(args.path_gt)
    np.save(args.path_gt + '.npy', np_gt_vec)





